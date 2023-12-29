#include "concrete/character/all.h"
#include "frame/event.h"
#include "frame/global/uibuff.h"
#include "frame/global/uiskill.h"
#include "program/log.h"
#include "program/settings.h"
#include "thread/data_models/task.h"
#include "thread/web_handler.h"
#include <filesystem>
#include <memory>
#include <nlohmann/json.hpp>

using namespace ns_thread;
using json = nlohmann::json;

static int calculate(const DMTask &arg);
static int output(const DMTask &arg, std::filesystem::path resfile);

static inline int getInt(bool &stat, const nlohmann::json &j, const std::string &key) {
    if (j.contains(key) && j[key].is_number_integer()) {
        return j[key];
    }
    stat = false;
    return 0;
}

static inline bool getBool(bool &stat, const nlohmann::json &j, const std::string &key) {
    if (j.contains(key) && j[key].is_boolean()) {
        return j[key];
    }
    stat = false;
    return false;
}

bool WebHandler::task(const std::string &jsonstr) {
    std::filesystem::path p_res = ns_program::Config::pExeDir / "res.tab";

    // TODO: check jsonstr
    json j    = json::parse(jsonstr);
    bool stat = true;

    std::unique_ptr<ns_frame::Player> player = ns_concrete::PlayerManager::create(ns_concrete::PlayerType::MjFysj, 0, 0);
    switch (j["attribute"]["id"].get<int>()) {
    case static_cast<int>(DMTask::AttributeType::jx3box):
        player->attrImportFromJX3BOX(j["attribute"]["data"].get<int>());
        break;
    default:
        return false;
    }

    bool useCustomMacro = j.contains("customMacro") && j["customMacro"].is_string();

    DMTask arg{
        .attrBackup     = player->chAttr,
        .delayNetwork   = getInt(stat, j, "delayNetwork"),
        .delayKeybord   = getInt(stat, j, "delayKeybord"),
        .fightTime      = getInt(stat, j, "fightTime"),
        .fightCount     = getInt(stat, j, "fightCount"),
        .tz_jn          = getBool(stat, j["effects"], "套装·技能"),
        .tz_tx          = getBool(stat, j["effects"], "套装·特效"),
        .dfm_y          = getBool(stat, j["effects"], "大附魔·腰"),
        .dfm_w          = getBool(stat, j["effects"], "大附魔·腕"),
        .dfm_x          = getBool(stat, j["effects"], "大附魔·鞋"),
        .wq_cw          = getBool(stat, j["effects"], "武器·橙武"),
        .useCustomMacro = useCustomMacro,
        .customMacro    = useCustomMacro ? j["customMacro"].get<std::string>() : "",
    };

#ifdef DEBUG
    ns_program::log_info.enable  = true;
    ns_program::log_error.enable = true;
    ns_program::log_error.output = true;
#endif

    auto first     = pool.enqueue(output, arg, p_res);
    int  timespend = first.get();
    std::cout << "第一次计算花费时间: " << timespend << "ms, 已将战斗记录保存至 res.tab" << std::endl;

#ifdef DEBUG
    ns_program::log_info.save();
    ns_program::log_error.save();
    ns_program::log_info.enable  = false;
    ns_program::log_error.enable = false;
    ns_program::log_error.output = false;
#endif

    std::vector<std::future<int>> futures;
    for (int i = 0; i < arg.fightCount; ++i) {
        futures.emplace_back(pool.enqueue(calculate, arg));
    }
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
    if (arg.tz_jn)
        player->skillrecipeAdd(948, 2);
    if (arg.tz_tx)
        player->skilleventAdd(1922);
    if (arg.dfm_y)
        player->skilleventAdd(1705);
    if (arg.dfm_w)
        player->skilleventAdd(1843);
    if (arg.dfm_x)
        player->skilleventAdd(2393);
    if (arg.wq_cw)
        player->skilleventAdd(2421);
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
