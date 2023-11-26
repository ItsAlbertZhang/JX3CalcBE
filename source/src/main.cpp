#include "frame/character/character.h"
#include "frame/static_lua.h"
#include "gdi.h"
#include "program/init.h"
#include <filesystem>
#include <iostream>
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
    ret = gdi::Interface::initLua(ns_framestatic::luaInit, ns_framestatic::luaFuncStaticToDynamic);
    std::cout << "initLua  = " << ret << std::endl;
    ret = gdi::Interface::initTab(static_cast<int>(gdi::Tab::COUNT));
    std::cout << "initTab  = " << ret << std::endl;

    // 如果成功加载 GameDataFetcher, current_path 会发生改变.
    std::cout << std::filesystem::current_path() << std::endl;

    // // 测试用例 1
    // sol::state lua;
    // lua.open_libraries(sol::lib::base);
    // lua.script("function foo() return 1 end");
    // sol::function foo = lua["foo"];
    // std::cout << "First call: " << int(foo()) << std::endl;

    // lua.script("function foo() return 2 end");
    // std::cout << "Second call: " << int(foo()) << std::endl;
    // sol::function foo2 = lua["foo"];
    // std::cout << "Foo2 call: " << int(foo2()) << std::endl;

    // 测试用例 2
    ns_frame::Player player;
    ns_frame::NPC npc;
    // player.LearnSkill(10242, 13); // 焚影圣诀
    player.target = &npc;
    player.chAttr.atSolarAttackPowerBase = 1000;
    player.LearnSkill(3963, 10); // 烈日斩
    player.CastSkill(3963, 10);
    while (!player.chSkill.skillQueue.empty()) {
        auto it = player.chSkill.skillQueue.front();
        player.chSkill.skillQueue.pop();
        player.CastSkill(it.skillID, it.skillLevel);
    }
    player.CastSkill(3963, 10);
    for (auto &it : player.chDamage.damageList) {
        std::cout << it.tick << " " << it.skillID << " " << it.skillLevel << " " << it.isCritical << " " << it.damage << " " << static_cast<int>(it.damageType) << std::endl;
    }

    // // 测试用例 3
    // class MyClass {
    // public:
    //     int a, b = 0;
    //     MyClass *func() {
    //         printf("%d in func\n", a);
    //         // return this;
    //         return nullptr;
    //     }
    // };
    // sol::state lua;
    // lua.open_libraries(sol::lib::base);
    // MyClass obj;
    // obj.a = 10;
    // lua.new_usertype<MyClass>("MyClass",
    //                           "a", &MyClass::a,
    //                           "b", &MyClass::b,
    //                           "func", &MyClass::func);
    // lua.script("function f(obj) if(obj:func()) then obj.b = 123 end end");
    // sol::protected_function luafunc = lua["f"];
    // MyClass *ptr = &obj;
    // sol::protected_function_result res = luafunc(ptr);
    // if (!res.valid()) {
    //     sol::error err = res;
    //     std::cout << "Lua error: " << err.what() << std::endl;
    // } else {
    //     printf("%d\n", obj.b);
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