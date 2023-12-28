#include "concrete/character/npc/npc124.h"
#include "concrete/character/player/mj_fysj.h"
#include "frame/character/derived/player.h"
#include "frame/character/property/buff.h"
#include "frame/event.h"
#include "frame/global/uibuff.h"
#include "frame/global/uiskill.h"
#include "frame/lua_runtime.h"
#include "frame/lua_static.h"
#include "gdi.h"
#include "program/log.h"
#include "program/settings.h"
#include "thread/pool.h"
#pragma warning(push, 0)
#include <crow.h>
#pragma warning(pop)
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#ifdef _WIN32
#include <Windows.h>
#endif

ns_frame::ChAttr      attr_backup;
int                   delay_network;
int                   delay_keybord;
int                   fight_count;
int                   fight_time;
std::filesystem::path p_api;
std::filesystem::path p_res;
bool                  use_costume_macro;

thread_local std::unique_ptr<ns_frame::MacroCustom> ptr_macro_custom;

void thread_init() {
    if (use_costume_macro) {
        ptr_macro_custom = std::make_unique<ns_frame::MacroCustom>(p_api);
    }
}

void thread_cleanup() {
    ns_frame::LuaFunc::clear();
}

bool tz_jn  = false;
bool tz_tx  = false;
bool dfm_yd = false;
bool dfm_hw = false;
bool dfm_xz = false;
bool wq_cw  = false;

void init_player(ns_frame::Player &player) {
    if (tz_jn)
        player.skillrecipeAdd(948, 2);
    if (tz_tx)
        player.skilleventAdd(1922);
    if (dfm_yd)
        player.skilleventAdd(1705);
    if (dfm_hw)
        player.skilleventAdd(1843);
    if (dfm_xz)
        player.skilleventAdd(2393);
    if (wq_cw)
        player.skilleventAdd(2421);
}

int thread_calculate() {
    ns_concrete::MjFysj fysj{delay_network, delay_keybord};
    ns_concrete::NPC124 npc124;
    ns_frame::Player   &player = fysj;
    ns_frame::NPC      &npc    = npc124;
    player.targetSelect        = &npc;
    player.macroCustom         = ptr_macro_custom.get();
    player.attrImportFromBackup(attr_backup);
    init_player(player);
    ns_frame::Event::clear();
    // auto start = std::chrono::steady_clock::now();
    player.macroRun();
    while (ns_frame::Event::now() < static_cast<ns_frame::event_tick_t>(1024 * fight_time)) {
        bool ret = ns_frame::Event::run();
        if (!ret)
            break;
    }
    // auto end = std::chrono::steady_clock::now();

    unsigned long long sumDamage = 0;
    for (auto &it : player.chDamage.damageList) {
        sumDamage += it.damageExcept;
    }
    return static_cast<int>(sumDamage / fight_time);
    // return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
}

int thread_output() {
    std::ofstream ofs(p_res);
    auto          start = std::chrono::steady_clock::now();

    ns_concrete::MjFysj fysj{delay_network, delay_keybord};
    ns_concrete::NPC124 npc124;
    ns_frame::Player   &player = fysj;
    ns_frame::NPC      &npc    = npc124;
    player.targetSelect        = &npc;
    player.macroCustom         = ptr_macro_custom.get();
    player.attrImportFromBackup(attr_backup);
    init_player(player);
    ns_frame::Event::clear();
    player.macroRun();
    while (ns_frame::Event::now() < static_cast<ns_frame::event_tick_t>(1024 * fight_time)) {
        bool ret = ns_frame::Event::run();
        if (!ret)
            break;
    }

    auto end = std::chrono::steady_clock::now();

    ofs << "time\ttype\tID\tlv\tdmgBase\tdmgCri\tdmgExp\tcriRate\tname\n";
    ns_frame::event_tick_t presentCurr = 0;
    for (auto &it : player.chDamage.damageList) {
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
    return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
}

int main(int argc, char *argv[]) {

    // 如果是 Windows 操作系统, 将控制台编码设置为 UTF-8, 以便输出中文.
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    bool ret;
    // 初始化程序
    ns_program::init(argc, argv);
    // 初始化接口
    ret = gdi::init(
        ns_program::Config::pJX3,
        ns_program::Config::pUnpack,
        ns_frame::luaInit,
        ns_frame::luaFuncStaticToDynamic,
        static_cast<int>(gdi::Tab::COUNT)
    );
    if (!ret)
        return 0;

    crow::SimpleApp app;

    CROW_ROUTE(app, "/<string>")
    ([](const std::string &user_name) {
        std::cout << "message from " + user_name << std::endl;
        return "hello, " + user_name;
    });

    app.port(12897).multithreaded().run();

    p_res = ns_program::Config::pExeDir / "res.tab";
    p_api = ns_program::Config::pExeDir / "api.lua";
    ns_frame::MacroCustom macroCustom(p_api);

    use_costume_macro = macroCustom.lua["UseCustomMacro"].get<bool>();
    fight_count       = macroCustom.lua["FightCount"].get<int>();
    fight_time        = macroCustom.lua["FightTime"].get<int>();
    delay_network     = macroCustom.lua["DelayBase"].get<int>();
    delay_keybord     = macroCustom.lua["DelayRand"].get<int>();
    tz_jn             = macroCustom.lua["TZ_JN"].get<bool>();
    tz_tx             = macroCustom.lua["TZ_TX"].get<bool>();
    dfm_yd            = macroCustom.lua["DFM_YD"].get<bool>();
    dfm_hw            = macroCustom.lua["DFM_HW"].get<bool>();
    dfm_xz            = macroCustom.lua["DFM_XZ"].get<bool>();
    wq_cw             = macroCustom.lua["WQ_CW"].get<bool>();
    if (fight_count <= 0)
        fight_count = 300;
    if (fight_time <= 0)
        fight_time = 300;
    if (delay_network <= 0)
        delay_network = 45;
    if (delay_keybord <= 0)
        delay_keybord = 20;

    ns_concrete::MjFysj fysj{delay_network, delay_keybord};
    ns_frame::Player   &player = fysj;
    macroCustom.attrInit(player);
    attr_backup = player.attrExport();

#ifdef DEBUG
    ns_program::log_info.enable  = true;
    ns_program::log_error.enable = true;
    ns_program::log_error.output = true;
#endif

    ns_thread::Pool pool(thread_init, thread_cleanup);
    auto            first     = pool.enqueue(thread_output);
    int             timespend = first.get();
    std::cout << "第一次计算花费时间: " << timespend << "ms, 已将战斗记录保存至 res.tab" << std::endl;

#ifdef DEBUG
    ns_program::log_info.save();
    ns_program::log_error.save();
    ns_program::log_info.enable  = false;
    ns_program::log_error.enable = false;
    ns_program::log_error.output = false;
#endif

    std::vector<std::future<int>> futures;
    for (int i = 0; i < fight_count; ++i) {
        futures.emplace_back(pool.enqueue(thread_calculate));
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
            std::cout << "\r(" << idx << "/" << fight_count << ")   当前速度: " << idx - idxBeforeSleep << " 次计算/s   " << std::flush;
            idxBeforeSleep = idx;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    auto end = std::chrono::steady_clock::now();

    const unsigned int corecnt = std::thread::hardware_concurrency();
    std::cout << "\r                                                                                                    \r";
    std::cout << "平均 DPS: " << damageAvg / fight_count << std::endl;
    double timespendAvg = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() * corecnt / static_cast<double>(fight_count));
    std::cout << "平均每次计算所需时间: " << std::fixed << std::setprecision(2) << timespendAvg << " ms / " << corecnt << " 并发数 = "
              << std::fixed << std::setprecision(2) << timespendAvg / corecnt << " ms" << std::endl;

#ifdef _WIN32
    system("pause");
#endif

    thread_cleanup();

    return 0;
}
