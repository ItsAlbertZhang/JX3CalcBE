#include "concrete/character/all.h"
#include "frame/event.h"
#include "frame/global/uibuff.h"
#include "frame/global/uiskill.h"
#include "global/log.h"
#include "global/settings.h"
#include "program/task.h"
#include "program/thread_web.h"
#include <filesystem>
#include <memory>

using namespace ns_program;

static int calculate(const DMTask &arg);
static int output(const DMTask &arg, std::filesystem::path resfile);

bool Web::task(const std::string &jsonstr) {
    std::filesystem::path p_res = ns_global::Config::pExeDir / "res.tab";

#ifdef DEBUG
    ns_global::log_info.enable  = true;
    ns_global::log_error.enable = true;
    ns_global::log_error.output = true;
#endif

    auto argptr = DMTask::create(jsonstr);
    if (argptr == nullptr) {
        return false;
    }
    DMTask &arg = *argptr;

    auto first     = pool.enqueue("taska", 1, output, arg, p_res);
    int  timespend = first[0].get();
    std::cout << "第一次计算花费时间: " << timespend << "ms, 已将战斗记录保存至 res.tab" << std::endl;

#ifdef DEBUG
    ns_global::log_info.save();
    ns_global::log_error.save();
    ns_global::log_info.enable  = false;
    ns_global::log_error.enable = false;
    ns_global::log_error.output = false;
#endif

    auto               futures        = pool.enqueue("taskb", arg.fightCount, calculate, arg);
    unsigned long long damageAvg      = 0;
    int                idx            = 0;
    int                idxBeforeSleep = 0;
    auto               start          = std::chrono::steady_clock::now();
    while (idx >= 0 && static_cast<std::vector<int>::size_type>(idx) < futures.size()) {
        if (futures[idx].wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            damageAvg += futures[idx].get();
            idx++;
        } else {
            std::cout << "\r(" << idx << "/" << arg.fightCount << ")   当前速度: " << idx - idxBeforeSleep << " 次计算/s   " << std::flush;
            idxBeforeSleep = idx;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    auto end = std::chrono::steady_clock::now();

    const unsigned int corecnt = std::thread::hardware_concurrency();
    std::cout << "\r                                                                                                    \r";
    std::cout << "平均 DPS: " << damageAvg / arg.fightCount << std::endl;
    double timespendAvg = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() * corecnt / static_cast<double>(arg.fightCount));
    std::cout << "平均每次计算所需时间: " << std::fixed << std::setprecision(2) << timespendAvg << " ms / " << corecnt << " 并发数 = "
              << std::fixed << std::setprecision(2) << timespendAvg / corecnt << " ms" << std::endl;
    return true;
}

static auto calc(const DMTask &arg) {
    static thread_local std::unordered_map<size_t, std::unique_ptr<ns_frame::MacroCustom>> map;

    std::unique_ptr<ns_frame::Player> player = ns_concrete::PlayerManager::create(ns_concrete::PlayerType::MjFysj, arg.delayNetwork, arg.delayKeybord);
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
    auto start  = std::chrono::steady_clock::now();
    auto player = calc(arg);
    auto end    = std::chrono::steady_clock::now();

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
    std::cout << "第一次计算 DPS: " << static_cast<int>(sumDamage / arg.fightTime) << std::endl;
    return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
}
