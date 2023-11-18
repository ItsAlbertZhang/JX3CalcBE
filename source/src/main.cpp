#include "frame/character.h"
#include "frame/lua_interface.h"
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
    ret = gdi::Interface::initGameData(ns_program::Config::pJX3, ns_program::Config::pUnpack);
    std::cout << "initGameData = " << ret << std::endl;
    ret = gdi::Interface::initLua(ns_frame::LuaDependence::lua_init, ns_frame::LuaDependence::staticFuncNeedConvert);
    std::cout << "initLua  = " << ret << std::endl;
    ret = gdi::Interface::initTab(static_cast<int>(gdi::Tab::COUNT));
    std::cout << "initTab  = " << ret << std::endl;

    // 如果成功加载 GameDataFetcher, current_path 会发生改变.
    std::cout << std::filesystem::current_path() << std::endl;

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
    // gdi::Interface::tabSelect(gdi::Tab::buff, arg1);
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
    // gdi::Interface::tabSelect(gdi::Tab::skills, arg2);
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
#ifdef DEBUG
    std::cout << "Press any key to exit..." << std::endl;
#endif
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