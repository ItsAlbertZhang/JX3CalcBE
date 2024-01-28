#include "concrete/character/all.h"
#include "frame/event.h"
#include "frame/global/uibuff.h"
#include "frame/global/uiskill.h"
#include "global/config.h"
#include "global/constexpr_channelinterval.h"
#include "global/constexpr_log.h"
#include "program/task.h"
#include "program/thread_web.h"
#include <asio/co_spawn.hpp>
#include <chrono>
#include <filesystem>
#include <memory>

#ifdef _WIN32
#include <format>
namespace fmt = std;
#else
#include <fmt/core.h>
#endif

using namespace ns_program;

static int calculate(const DMTask &arg);
static int output(const DMTask &arg, std::filesystem::path resfile);

std::string Web::urlTask(const std::string &jsonstr) {
    std::filesystem::path p_res = nsg_config::pExeDir / "res.tab";

#ifdef D_CONSTEXPR_LOG
    nsgc_log::info.enable  = true;
    nsgc_log::error.enable = true;
    nsgc_log::error.output = true;
#endif
    auto argptr = DMTask::create(jsonstr);
    if (argptr == nullptr) {
        return "";
    }
#ifdef D_CONSTEXPR_LOG
    nsgc_log::info.enable  = false;
    nsgc_log::error.enable = false;
    nsgc_log::error.output = false;
    nsgc_log::info.save();
    nsgc_log::error.save();
#endif
    std::string id = genID();
    Task       &it =
        tasks.emplace(
                 std::piecewise_construct,
                 std::forward_as_tuple(id),
                 std::forward_as_tuple(id, std::move(*argptr))
        )
            .first->second;
    // 原地插入. 通过这种方式插入的 key 和 value, 不是构造后移动至容器, 而是直接在容器内构造.
    argptr.reset();
    pool.emplace(id, 1, it.futures, output, it.data, p_res);
    pool.emplace(id, it.data.fightCount, it.futures, calculate, it.data);

    return id;
}

static asio::awaitable<void> taskCoroutine(asio::io_context &io, crow::websocket::connection &conn, Task &task) {
    asio::steady_timer         timer(io);
    const std::chrono::seconds interval{1};

    // 处理第一个 output 任务
    int timespend = task.futures[0].get();
    conn.send_text(fmt::format("第一次计算花费时间: {}ms, 已将战斗记录保存至 res.tab\n", timespend));
    for (size_t i = 1; i < task.futures.size() && !task.stop.load(); i++) {
    }
    using ull           = unsigned long long;
    ull  damageAvg      = 0;
    int  idx            = 1; // 从 1 开始, 因为 0 已经处理过了
    int  idxBeforeSleep = 1;
    auto start          = std::chrono::steady_clock::now();
    while (idx >= 0 && static_cast<std::vector<int>::size_type>(idx) < task.futures.size() && !task.stop.load()) [[likely]] {
        if (task.futures[idx].wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            damageAvg += task.futures[idx].get();
            idx++;
        } else {
            if (idx > 1) [[likely]]
                conn.send_text(fmt::format("({}/{})    当前速度: {} 次计算/s. 到目前为止的平均 DPS: {}\n", idx, task.data.fightCount, idx - idxBeforeSleep, damageAvg / (idx - 1)));
            idxBeforeSleep = idx;
            timer.expires_after(interval);
            co_await timer.async_wait(asio::use_awaitable);
        }
    }
    if (!task.stop.load()) {
        auto end = std::chrono::steady_clock::now();

        const unsigned int corecnt      = std::thread::hardware_concurrency();
        double             timespendAvg = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() * corecnt / static_cast<double>(task.data.fightCount));
        conn.send_text(fmt::format(
            "平均 DPS: {}\n平均每次计算所需时间: {} ms.\n{} 核心并行计算下, 等效计算时间 {} ms.\n",
            damageAvg / task.data.fightCount,
            timespendAvg,
            corecnt,
            timespendAvg / corecnt
        ));
    }
}

void Web::urlTaskWs_onMessage(crow::websocket::connection &conn, const std::string &data, bool is_binary) {
    if (is_binary || !tasks.contains(data))
        return;
    Task &task = tasks.at(data);
    wsmap.emplace(&conn, &task);
    asio::co_spawn(io, taskCoroutine(io, conn, task), asio::detached);
}

void Web::urlTaskWs_onClose(crow::websocket::connection &conn) {
    if (!wsmap.contains(&conn))
        return;
    Task &task = *wsmap.at(&conn);
    task.stop.store(true);
    pool.erase(task.id);
    tasks.erase(task.id);
    wsmap.erase(&conn);
}

static auto calc(const DMTask &arg) {
    static thread_local std::unordered_map<size_t, std::unique_ptr<ns_frame::MacroCustom>> map;

    std::unique_ptr<ns_frame::Player> player = ns_concrete::PlayerManager::create(ns_concrete::PlayerType::MjFysj, arg.delayNetwork, arg.delayKeyboard);
    std::unique_ptr<ns_frame::NPC>    npc    = ns_concrete::NPCManager::create(ns_concrete::NPCType::NPC124);
    player->targetSelect                     = npc.get();
    if (arg.useCustomMacro) {
        if (map.find(arg.custom_macro_hash) == map.end()) {
            map.emplace(arg.custom_macro_hash, std::make_unique<ns_frame::MacroCustom>(arg.customMacro));
        }
        player->macroCustom = map.at(arg.custom_macro_hash).get();
    }
    player->attrImportFromBackup(arg.attrBackup);
    for (auto &it : arg.effects) {
        it->active(player.get());
    }
    ns_frame::Event::clear();
    player->macroRun();
    while (ns_frame::Event::now() < static_cast<ns_frame::event_tick_t>(1024 * arg.fightTime)) {
        bool ret = ns_frame::Event::run();
        if (!ret)
            break;
    }

    return player;
}

static int calculate(const DMTask &arg) {
    auto player = calc(arg);

    unsigned long long sumDamage = 0;
    for (auto &it : player->chDamage.damageList) {
        sumDamage += it.damageExcept;
    }
    return static_cast<int>(sumDamage / arg.fightTime);
}

static int output(const DMTask &arg, std::filesystem::path resfile) {
#ifdef D_CONSTEXPR_LOG
    nsgc_log::info.enable  = true;
    nsgc_log::error.enable = true;
    nsgc_log::error.output = true;
#endif
#ifdef D_CONSTEXPR_CHANNELINTERVAL
    nsgc_channelinterval::enable = true;
#endif
    auto start  = std::chrono::steady_clock::now();
    auto player = calc(arg);
    auto end    = std::chrono::steady_clock::now();
#ifdef D_CONSTEXPR_CHANNELINTERVAL
    nsgc_channelinterval::enable = false;
    nsgc_channelinterval::save();
#endif
#ifdef D_CONSTEXPR_LOG
    nsgc_log::info.enable  = false;
    nsgc_log::error.enable = false;
    nsgc_log::error.output = false;
    nsgc_log::info.save();
    nsgc_log::error.save();
#endif

    std::ofstream ofs{resfile};
    ofs << "time\ttype\tID\tlv\tdmgBase\tdmgCri\tdmgExp\tcriRate\tname\n";
    ns_frame::event_tick_t presentCurr = 0;
    for (auto &it : player->chDamage.damageList) {
        std::string name;
        switch (it.source) {
        case ns_frame::DamageSource::skill: {
            const ns_frame::UISkill &skill = ns_frame::UISkillManager::get(it.id, it.level);
            name                           = skill.Name;
        } break;
        case ns_frame::DamageSource::buff: {
            const ns_frame::UIBuff &buff = ns_frame::UIBuffManager::get(it.id, it.level);
            name                         = buff.Name;
        } break;
        default:
            break;
        }
        if (it.tick != presentCurr) {
            presentCurr = it.tick;
            ofs << "\n";
        }
        ofs << std::fixed << std::setprecision(2) << it.tick / 1024.0 << "s\t"
            << static_cast<int>(it.damageType) << "\t"
            << it.id << "\t" << it.level << "\t"
            << it.damageBase << "\t" << it.damageCritical << "\t" << it.damageExcept << "\t" << it.criticalRate << "\t"
            << name << "\n";
    }
    ofs.close();
    unsigned long long sumDamage = 0;
    for (auto &it : player->chDamage.damageList) {
        sumDamage += it.damageExcept;
    }
    // std::cout << "第一次计算 DPS: " << static_cast<int>(sumDamage / arg.fightTime) << std::endl;
    return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
}
