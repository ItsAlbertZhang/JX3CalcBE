#include "modules/task.h"
#include "concrete/character/character.h"
#include "concrete/effect/effect.h"
#include "frame/event.h"
#include "frame/global/uibuff.h"
#include "frame/global/uiskill.h"
#include "plugin/channelinterval.h"
#include "plugin/log.h"
#include "utils/config.h"
#include <asio/co_spawn.hpp>
#include <chrono>
#include <filesystem>
#include <memory>
#include <optional>

#ifdef _WIN32
#include <format>
namespace fmt = std;
#else
#include <fmt/core.h>
#endif

using namespace ns_modules::web::task;

void ns_modules::web::task::server::run(crow::websocket::connection *conn) {
    asio::co_spawn(
        io,
        [&]() -> asio::awaitable<void> {
            asio::steady_timer timer(io);
            while (!iostop.load()) {
                timer.expires_after(std::chrono::seconds(1));
                co_await timer.async_wait(asio::use_awaitable);
            }
        },
        asio::detached
    ); // 用于保持 io.run() 的运行
    threadIO = std::thread([&]() {
        io.run();
    });

    ns_modules::web::task::server::conn = conn;
}

void ns_modules::web::task::server::stop() {
    for (auto &task : taskMap)
        task::stop(task.first);
    iostop.store(true);
    threadIO.join();
}

static int         calculate(const Data &arg);
static int         output(const Data &arg, std::filesystem::path resfile);
static std::string genID(int length = 6);

Response ns_modules::web::task::validate(const std::string &jsonstr) {
    using json = nlohmann::json;
    // 解析 json
    json j;
    try {
        j = json::parse(jsonstr);
    } catch (...) {
        return Response{
            .status  = false,
            .content = "json parse error",
        };
    }
    // 检查字段
    // 1. 检查必须字段
    static const std::vector<std::string> required{
        "player", "delayNetwork", "delayKeyboard", "fightTime", "fightCount", "attribute", "effects"
    };
    for (auto &it : required) {
        if (!j.contains(it)) {
            return Response{
                .status  = false,
                .content = fmt::format("missing required field: {}", it),
            };
        }
    }
    // 2. 分别检查字段值
    // 2.1 检查 player
    if (!j["player"].is_string() || !ns_concrete::refPlayerType.contains(j["player"].get<std::string>())) {
        return Response{
            .status  = false,
            .content = "player invalid",
        };
    }
    // 2.2 检查 delayNetwork, delayKeyboard, fightTime, fightCount
    if (!j["delayNetwork"].is_number_integer() || j["delayNetwork"].get<int>() < 0 ||
        j["delayNetwork"].get<int>() > ns_utils::config::taskdata::maxDelayNetwork) {
        return Response{
            .status  = false,
            .content = "delayNetwork invalid",
        };
    }
    if (!j["delayKeyboard"].is_number_integer() || j["delayKeyboard"].get<int>() < 0 ||
        j["delayKeyboard"].get<int>() > ns_utils::config::taskdata::maxDelayKeyboard) {
        return Response{
            .status  = false,
            .content = "delayKeyboard invalid",
        };
    }
    if (!j["fightTime"].is_number_integer() || j["fightTime"].get<int>() < 0 ||
        j["fightTime"].get<int>() > ns_utils::config::taskdata::maxFightTime) {
        return Response{
            .status  = false,
            .content = "fightTime invalid",
        };
    }
    if (!j["fightCount"].is_number_integer() || j["fightCount"].get<int>() < 0 ||
        j["fightCount"].get<int>() > ns_utils::config::taskdata::maxFightCount) {
        return Response{
            .status  = false,
            .content = "fightCount invalid",
        };
    }
    // 2.3 检查 attribute
    if (!j["attribute"].contains("method") || !j["attribute"]["method"].is_string() ||
        !refAttributeType.contains(j["attribute"]["method"].get<std::string>()) ||
        !j["attribute"].contains("data") || !j["attribute"]["data"].is_object()) {
        return Response{
            .status  = false,
            .content = "attribute method invalid",
        };
    }
    AttributeType type = refAttributeType.at(j["attribute"]["method"].get<std::string>());
    switch (type) {
    case AttributeType::jx3box: {
        if (!j["attribute"]["data"].contains("pzid") || !j["attribute"]["data"]["pzid"].is_string()) {
            return Response{
                .status  = false,
                .content = "attribute data invalid",
            };
        }
    } break;
    default:
        return Response{
            .status  = false,
            .content = "attribute data invalid",
        };
        break;
    }
    // 2.4 检查 effects
    if (!j["effects"].is_array()) {
        return Response{
            .status  = false,
            .content = "effects invalid",
        };
    }
    for (auto &it : j["effects"]) {
        if (!it.is_string() || !ns_concrete::refEffectType.contains(it.get<std::string>())) {
            return Response{
                .status  = false,
                .content = fmt::format("effects invalid: {}", it.get<std::string>()),
            };
        }
    }
    // 检查完毕, 返回 true
    return Response{
        .status = true,
    };
}

static std::optional<Data> createTaskData(const nlohmann::json &j) {
    auto playerType = ns_concrete::refPlayerType.at(j["player"].get<std::string>());
    // player. 此处的 player 仅用作属性导入, 因此无需设置 delayNetwork 和 delayKeyboard
    auto player     = ns_concrete::createPlayer(playerType, 0, 0);

    auto attrType = refAttributeType.at(j["attribute"]["method"].get<std::string>());
    switch (attrType) {
    case AttributeType::jx3box: {
        std::string pzid = j["attribute"]["data"]["pzid"].get<std::string>();
        if (!player->attrImportFromJX3BOX(pzid)) {
            return std::nullopt;
        }
    } break;
    default:
        return std::nullopt;
        break;
    }

    std::vector<std::shared_ptr<ns_concrete::EffectBase>> effectList;
    for (auto &x : j["effects"].items()) {
        effectList.emplace_back(ns_concrete::createEffect(ns_concrete::refEffectType.at(x.value().get<std::string>())));
    }

    return Data{
        .delayNetwork   = j["delayNetwork"].get<int>(),
        .delayKeyboard  = j["delayKeyboard"].get<int>(),
        .fightTime      = j["fightTime"].get<int>(),
        .fightCount     = j["fightCount"].get<int>(),
        .useCustomMacro = j.contains("customMacro"),
        .customMacro    = j.contains("customMacro") ? j["customMacro"].get<std::string>() : std::string(),
        .attrBackup     = player->chAttr,
        .effects        = std::move(effectList),
    }; // 返回值隐式类型转换为 std::optional
}

static void asyncStop(Task &task) {
    using namespace ns_modules::web::task;
    task.stop.store(true);
    task.futures.clear();
    server::pool.erase(task.id);
    server::taskMap.erase(task.id);
}

static asio::awaitable<void> asyncRun(asio::io_context &io, Task &task) {
    asio::steady_timer          timer(io);
    const std::chrono::seconds  interval{1};
    const std::filesystem::path p_res = ns_utils::config::pExeDir / "res.tab";
    using namespace ns_modules::web::task;
    // 在线程池中创建任务
    server::pool.emplace(task.id, 1, task.futures, output, task.data, p_res);
    server::pool.emplace(task.id, task.data.fightCount, task.futures, calculate, task.data);
    // 处理第一个 output 任务
    int timespend = task.futures[0].get();
    server::conn->send_text(fmt::format("第一次计算花费时间: {}ms, 已将战斗记录保存至 res.tab\n", timespend));
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
            // // 发送进度并异步等待前, 先检查是否需要停止
            if (task.stop.load()) [[unlikely]] {
                asyncStop(task);
                co_return;
            }
            if (idx > 1) [[likely]]
                server::conn->send_text(fmt::format("({}/{})    当前速度: {} 次计算/s. 到目前为止的平均 DPS: {}\n", idx, task.data.fightCount, idx - idxBeforeSleep, damageAvg / (idx - 1)));
            idxBeforeSleep = idx;
            timer.expires_after(interval);
            co_await timer.async_wait(asio::use_awaitable);
        }
    }
    if (!task.stop.load()) {
        auto end = std::chrono::steady_clock::now();

        const unsigned int corecnt      = std::thread::hardware_concurrency();
        double             timespendAvg = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() * corecnt / static_cast<double>(task.data.fightCount));
        server::conn->send_text(fmt::format(
            "平均 DPS: {}\n平均每次计算所需时间: {} ms.\n{} 核心并行计算下, 等效计算时间 {} ms.\n",
            damageAvg / task.data.fightCount,
            timespendAvg,
            corecnt,
            timespendAvg / corecnt
        ));
    }
    asyncStop(task);
}

Response ns_modules::web::task::create(const std::string &jsonstr) {
    if (server::conn == nullptr) {
        return Response{
            .status  = false,
            .content = "websocket not connected.",
        };
    }

    // 验证 json
    auto res = validate(jsonstr);
    if (!res.status) [[unlikely]]
        return res;
    // 验证后, 使用时无需再次验证
    using json    = nlohmann::json;
    json j        = json::parse(jsonstr);
    auto taskdata = createTaskData(j);
    if (!taskdata.has_value()) [[unlikely]]
        return Response{
            .status  = false,
            .content = "An error occurred while creating task data. Please try again later.",
        };

    auto  id = genID();
    auto &it = server::taskMap.emplace(id, std::make_unique<Task>(id, std::move(taskdata.value()))).first->second;
    asio::co_spawn(server::io, asyncRun(server::io, *it), asio::detached);
    return Response{
        .status  = true,
        .content = id,
    };
}

void ns_modules::web::task::stop(std::string id) {
    if (!server::taskMap.contains(id)) [[unlikely]]
        return;
    server::taskMap.at(id)->stop.store(true); // 真正的停止操作在 asyncRun 中
}

static auto calc(const Data &arg) {
    static thread_local std::unordered_map<size_t, std::unique_ptr<ns_frame::MacroCustom>> map;

    std::unique_ptr<ns_frame::Player> player = ns_concrete::createPlayer(ns_concrete::PlayerType::MjFysj, arg.delayNetwork, arg.delayKeyboard);
    std::unique_ptr<ns_frame::NPC>    npc    = ns_concrete::createNPC(ns_concrete::NPCType::NPC124);
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

static int calculate(const Data &arg) {
    auto player = calc(arg);

    unsigned long long sumDamage = 0;
    for (auto &it : player->chDamage.damageList) {
        sumDamage += it.damageExcept;
    }
    return static_cast<int>(sumDamage / arg.fightTime);
}

static int output(const Data &arg, std::filesystem::path resfile) {
#ifdef D_CONSTEXPR_LOG
    ns_plugin::log::info.enable  = true;
    ns_plugin::log::error.enable = true;
    ns_plugin::log::error.output = true;
#endif
#ifdef D_CONSTEXPR_CHANNELINTERVAL
    ns_plugin::channelinterval::enable = true;
#endif
    auto start  = std::chrono::steady_clock::now();
    auto player = calc(arg);
    auto end    = std::chrono::steady_clock::now();
#ifdef D_CONSTEXPR_CHANNELINTERVAL
    ns_plugin::channelinterval::enable = false;
    ns_plugin::channelinterval::save();
#endif
#ifdef D_CONSTEXPR_LOG
    ns_plugin::log::info.enable  = false;
    ns_plugin::log::error.enable = false;
    ns_plugin::log::error.output = false;
    ns_plugin::log::info.save();
    ns_plugin::log::error.save();
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

static std::string genID(int length) {
    const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device              random_device;
    std::mt19937                    generator(random_device());
    std::uniform_int_distribution<> distribution(0, static_cast<int>(CHARACTERS.size() - 1));

    std::string random_string;
    while (random_string.empty() || ns_modules::web::task::server::taskMap.contains(random_string)) {
        for (size_t i = 0; i < length; ++i) {
            random_string += CHARACTERS[distribution(generator)];
        }
    }
    return random_string;
}
