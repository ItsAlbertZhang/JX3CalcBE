#include "concrete/character/npc/npc124.h"
#include "concrete/character/player/mj_fysj.h"
#include "frame/event.h"
#include "frame/global/uibuff.h"
#include "frame/global/uiskill.h"
#include "frame/lua_runtime.h"
#include "frame/lua_static.h"
#include "gdi.h"
#include "program/init.h"
#include "program/log.h"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <random> //  std::rand()
#ifdef _WIN32
#include <Windows.h>
#endif

int main(int argc, char *argv[]) {

    // 如果是 Windows 操作系统, 将控制台编码设置为 UTF-8, 以便输出中文.
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    bool ret;
    // 初始化程序
    ns_program::Init::init(argc, argv);
    // 初始化接口
    ret = gdi::Interface::initGameData(ns_program::Config::pJX3, ns_program::Config::pUnpack);
    if (!ret)
        return 0;

    ret = gdi::Interface::initLua(ns_frame::luaInit, ns_frame::luaFuncStaticToDynamic);
    if (!ret)
        return 0;

    ret = gdi::Interface::initTab(static_cast<int>(gdi::Tab::COUNT));
    if (!ret)
        return 0;

#ifdef DEBUG
    ns_program::log_error.printLast = true;
    if (argc > 1) {
        if (strcmp(argv[1], "--log=info") == 0) {
            ns_program::log_info.printImmediately = true;
        }
    }
#endif

    // std::filesystem::path pAPI = ns_program::Config::pExeDir / "api.lua";
    int fighttime = 300;

    auto start = std::chrono::steady_clock::now();
    ns_concrete::ChPlyMjFysj fysj;
    ns_concrete::ChNpc124 npc124;
    ns_frame::Player &player = fysj;
    ns_frame::NPC &npc = npc124;
    player.targetSelect = &npc;
    player.attrImport(""); // TODO: temp
    player.macroRun();
    while (ns_frame::Event::now() < 1024 * fighttime) {
        ret = ns_frame::Event::run();
        if (!ret)
            break;
    }
    auto end = std::chrono::steady_clock::now();

    unsigned long long totalDamage = 0;
    std::cout << "tick\t"
              << "ID\t"
              << "lv\t"
              << "cri\t"
              << "dmg\t"
              << "type\t"
              << "name" << std::endl;
    ns_frame::event_tick_t presentCurr = 0;
    for (auto &it : player.chDamage.damageList) {
        std::string name;
        switch (it.source) {
        case ns_frame::DamageSource::skill: {
            const ns_frame::UISkill &skill = ns_frame::UISkillManager::get(it.id, it.level);
            name = skill.Name;
        } break;
        case ns_frame::DamageSource::buff: {
            const ns_frame::UIBuff &buff = ns_frame::UIBuffManager::get(it.id, it.level);
            name = buff.Name;
        } break;
        }
        if (it.tick != presentCurr) {
            presentCurr = it.tick;
            std::cout << std::endl;
        }
        std::cout << std::fixed << std::setprecision(2) << it.tick / 1024.0 << "s\t"
                  << it.id << "\t" << it.level << "\t" << it.isCritical << "\t"
                  << it.damage << "\t" << static_cast<int>(it.damageType) << "\t"
                  << name << std::endl;
        totalDamage += it.damage;
    }

    unsigned long long damageAvg = 0;
    std::chrono::milliseconds timeAvg = std::chrono::milliseconds(0);
    for (int i = 0; i < 50; i++) {
        start = std::chrono::steady_clock::now();
        ns_concrete::ChPlyMjFysj fysj;
        ns_concrete::ChNpc124 npc124;
        ns_frame::Player &player = fysj;
        ns_frame::NPC &npc = npc124;
        player.targetSelect = &npc;
        player.attrImport(""); // TODO: temp
        ns_frame::Event::clear();
        player.macroRun();
        while (ns_frame::Event::now() < 1024 * fighttime) {
            ret = ns_frame::Event::run();
            if (!ret)
                break;
        }
        end = std::chrono::steady_clock::now();

        unsigned long long sumDamage = 0;
        for (auto &it : player.chDamage.damageList) {
            sumDamage += it.damage;
        }
        std::cout << sumDamage / fighttime << "\t" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
        damageAvg += sumDamage / fighttime;
        timeAvg += std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    }

    std::cout << "第一次计算花费时间: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
    std::cout << "平均 DPS: " << damageAvg / 50 << std::endl;
    std::cout << "平均计算时间: " << timeAvg.count() / 50 << "ms" << std::endl;

    ns_frame::LuaFunc::clear();
    return 0;
}
