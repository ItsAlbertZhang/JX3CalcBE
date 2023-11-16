#include "frame/character.h"
#include "frame/lua.h"
#include "gdi.h"
#include "program/init.h"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sol2/sol.hpp>
#include <time.h>
#ifdef _WIN32
#include <Windows.h>
#endif

// 初始化变量, 暂时放在这里, 后续放到 thread 里面
thread_local gdi::InterfaceInstance *gdi::ptrInterface = nullptr; // 当前线程的接口实例指针

int main(int argc, char *argv[]) {

    // 如果是 Windows 操作系统, 将控制台编码设置为 UTF-8, 以便输出中文.
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    std::cout << "你好, 世界!" << std::endl;

    bool ret;

    // 初始化程序
    ns_program::Init::init(argc, argv);
    // 初始化接口
    ret = gdi::InterfaceInstance::initGameData(ns_program::Config::pJX3, ns_program::Config::pUnpack);
    std::cout << "initGameData = " << ret << std::endl;
    ret = gdi::InterfaceInstance::initLuaPreprocess(ns_frame::lua_init);
    std::cout << "initLuaPreprocess  = " << ret << std::endl;
    ret = gdi::InterfaceInstance::initPtrInterface(gdi::ptrInterface);
    std::cout << "initPtrInterface = " << ret << std::endl;

    // 如果成功加载 GameDataFetcher, current_path 会发生改变.
    std::cout << std::filesystem::current_path() << std::endl;

    // // 创建接口实例
    // gdi::InterfaceInstance gdiInstance;

    // // 测试用例 1
    // gdiInstance.luaExecuteFile("scripts\\skill\\明教\\明教_烈日斩.lua");
    // ns_interface::Skill skill;
    // printf("in main, skill is %p\n", &skill);
    // ns_interface::InterfaceSkill interfaceskill;
    // ns_interface::InterfaceSkill::current_skill = &skill;
    // sol::protected_function GetSkillLevelData = gdiInstance.luaGetFunction("GetSkillLevelData");
    // sol::protected_function_result result = GetSkillLevelData(interfaceskill);
    // if (!result.valid()) {
    //     sol::error err = result;
    //     std::cout << "GetSkillLevelData failed:\n"
    //               << err.what() << std::endl;
    // } else {
    //     std::cout << "skill.nCostSprintPower = " << interfaceskill.nCostSprintPower << std::endl;
    //     std::cout << "skill.nMinRadius = " << interfaceskill.nMinRadius << std::endl;
    //     std::cout << "skill.nMaxRadius = " << interfaceskill.nMaxRadius << std::endl;
    //     std::cout << "skill.nAngleRange = " << interfaceskill.nAngleRange << std::endl;
    //     std::cout << "skill.bFullAngleInAir = " << interfaceskill.bFullAngleInAir << std::endl;
    //     std::cout << "skill.nChannelInterval = " << interfaceskill.nChannelInterval << std::endl;
    //     std::cout << "skill.nWeaponDamagePercent = " << interfaceskill.nWeaponDamagePercent << std::endl;
    // }

    // 测试用例 2
    ns_frame::Player player;
    ns_frame::NPC npc;
    // player.LearnSkill(10242, 13); // 焚影圣诀
    player.LearnSkill(3963, 10); // 烈日斩
    player.CastSkill(3963, 10);

    // // 测试用例 3
    // clock_t start, end;

    // gdi::TabSelectType arg1, arg2;
    // arg1.emplace_back();
    // arg1[0]["ID"] = "112";
    // arg1[0]["Level"] = "5";
    // arg2.emplace_back();
    // arg2[0]["SkillID"] = "112";
    // start = clock();
    // gdiInstance.tabSelect(gdi::Tab::buff, arg1);
    // end = clock();
    // std::cout << std::endl
    //           << "tabSelect 1: " << (double)((end - start) * 1000) / CLOCKS_PER_SEC << "ms." << std::endl
    //           << std::endl;
    // for (auto &idx : arg1) {
    //     for (auto &j : idx) {
    //         std::cout << j.first << ":" << j.second << " # ";
    //     }
    //     std::cout << std::endl;
    // }
    // start = clock();
    // gdiInstance.tabSelect(gdi::Tab::skills, arg2);
    // end = clock();
    // std::cout << std::endl
    //           << "tabSelect 2: " << (double)((end - start) * 1000) / CLOCKS_PER_SEC << "ms." << std::endl
    //           << std::endl;
    // for (auto &idx : arg2) {
    //     for (auto &j : idx) {
    //         std::cout << j.first << ":" << j.second << " # ";
    //     }
    //     std::cout << std::endl;
    // }

    return 0;
}

// ---------- 测试代码 ----------

// class Character {
// public:
//     int id;
//     void CastSkill(int targetID) {
//         std::cout << "id: " << id << std::endl;
//         std::cout << "CastSkill: " << targetID << std::endl;
//     }
// };

// int main() {
//     sol::state lua;
//     lua.open_libraries(sol::lib::base);

//     // 注册 Character 类
//     lua.new_usertype<Character>("Character",
//                                 "id", &Character::id,
//                                 "CastSkill", &Character::CastSkill);

//     Character c;

//     // 在 lua 中定义一个函数, 该函数接受一个 Character 类的实例, 修改其 id, 并调用 CastSkill
//     lua.script(R"(
//         function f(c)
//             c.id = 123
//             c:CastSkill(456)
//         end
//     )");

//     // 调用 lua 中的函数
//     lua["f"](c);

//     return 0;
// }