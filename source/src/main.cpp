#include "frame/character/character.h"
#include "frame/event.h"
#include "frame/global/uibuff.h"
#include "frame/global/uiskill.h"
#include "frame/lua_runtime.h"
#include "frame/lua_static.h"
#include "gdi.h"
#include "program/init.h"
#include "program/log.h"
#include <chrono>
#include <cstdlib> //  std::rand()
#include <filesystem>
#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#endif

std::vector<sol::protected_function> macro;
bool set_by_lua = false;
int lua_delay = 0;

void SetNextMacroActive(int delay) {
    set_by_lua = true;
    lua_delay = delay;
}

void callbackCastSkill(void *self, void *param) {
    ns_frame::Character *player = static_cast<ns_frame::Character *>(self);
    sol::protected_function_result res = macro[player->macroIdx](player);
    if (!res.valid()) {
        sol::error err = res;
        std::cout << "Lua error: " << err.what() << std::endl;
    }
    ns_frame::event_tick_t delay;
    if (!set_by_lua) {
        ns_frame::event_tick_t publicCooldownOver = player->chCooldown.cooldownList.at(player->publicCooldownID).tickOver;
        ns_frame::event_tick_t now = ns_frame::Event::now();
        delay = publicCooldownOver > now ? publicCooldownOver - now : 0; // 当前无技能可放
    } else {
        set_by_lua = false;
        delay = lua_delay;
    }
    delay += player->delayBase;
    delay += std::rand() % player->delayRand;
    ns_frame::Event::add(delay, callbackCastSkill, self, nullptr);
}

int main(int argc, char *argv[]) {

    // 如果是 Windows 操作系统, 将控制台编码设置为 UTF-8, 以便输出中文.
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    // std::cout << "你好, 世界!" << std::endl;

    bool ret;

    // 初始化程序
    ns_program::Init::init(argc, argv);
    // 初始化接口
    ret = gdi::Interface::initGameData(ns_program::Config::pJX3, ns_program::Config::pUnpack);
    // std::cout << "initGameData = " << ret << std::endl;
    if (!ret)
        return 0;

    ret = gdi::Interface::initLua(ns_frame::luaInit, ns_frame::luaFuncStaticToDynamic);
    // std::cout << "initLua  = " << ret << std::endl;
    if (!ret)
        return 0;

    ret = gdi::Interface::initTab(static_cast<int>(gdi::Tab::COUNT));
    // std::cout << "initTab  = " << ret << std::endl;
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

    // 如果成功加载 GameDataFetcher, current_path 会发生改变.
    // std::cout << std::filesystem::current_path() << std::endl;

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
    std::filesystem::path pAPI = ns_program::Config::pExeDir / "api.lua";
    sol::state lua;
    lua.open_libraries(sol::lib::base);
    ns_frame::luaInit(lua);
    lua.script_file(pAPI.string());

    ns_frame::Character player;
    ns_frame::Character npc;
    player.isPlayer = true;
    player.targetSelect = &npc;

    lua.set_function("SetNextMacroActive", SetNextMacroActive);

    sol::protected_function_result res = lua["Init"](player, npc);
    if (!res.valid()) {
        sol::error err = res;
        std::cout << "Lua error: " << err.what() << std::endl;
    }
    for (int i = 0; i < player.macroNum; i++) {
        macro.push_back(lua["Macro" + std::to_string(i)]);
    }

    res = lua["Ready"](player);
    if (!res.valid()) {
        sol::error err = res;
        std::cout << "Lua error: " << err.what() << std::endl;
    }

    // player.skillLearn(10242, 13); // 焚影圣诀
    // player.dwKungfuID = 10242;
    // player.skillActive(10242);

    // player.skillLearn(3962, 33); // 赤日轮
    // player.skillLearn(3963, 32); // 烈日斩
    // player.skillLearn(3966, 1);  // 生死劫
    // player.skillLearn(3967, 32); // 净世破魔击
    // player.skillLearn(3959, 24); // 幽月轮
    // player.skillLearn(3960, 18); // 银月斩
    // player.skillLearn(3969, 1);  // 光明相
    // player.skillLearn(3974, 1);  // 暗尘弥散
    // player.skillLearn(3979, 29); // 驱夜断愁

    // player.skillLearn(5972, 1);  // 腾焰飞芒
    // player.skillLearn(18279, 1); // 净身明礼
    // player.skillLearn(22888, 1); // 诛邪镇魔
    // player.skillLearn(22890, 1); // 诛邪镇魔, 主动
    // player.skillLearn(6717, 1);  // 无明业火
    // player.skillLearn(34383, 1); // 明光恒照
    // player.skillLearn(34395, 1); // 日月同辉
    // player.skillLearn(34372, 1); // 靡业报劫
    // player.skillLearn(17567, 1); // 用晦而明
    // player.skillLearn(25166, 1); // 净体不畏
    // player.skillLearn(34378, 1); // 降灵尊
    // player.skillLearn(34347, 1); // 悬象著明
    // player.skillLearn(34370, 1); // 日月齐光

    // player.skillActive(5972);
    // player.skillActive(18279);
    // player.skillActive(22888);
    // player.skillActive(6717);
    // player.skillActive(34383);
    // player.skillActive(34395);
    // player.skillActive(34372);
    // player.skillActive(17567);
    // player.skillActive(25166);
    // player.skillActive(34378);
    // // player.skillActive(34347);
    // player.skillActive(34370);

    // player.chSkillRecipe.add(1005, 1); // 赤日轮, 会心提高4%
    // player.chSkillRecipe.add(999, 1);  // 赤日轮, 伤害提高3%
    // player.chSkillRecipe.add(1000, 1); // 赤日轮, 伤害提高4%
    // player.chSkillRecipe.add(1001, 1); // 赤日轮, 伤害提高5%

    // player.chSkillRecipe.add(1011, 1); // 烈日斩, 会心提高4%
    // player.chSkillRecipe.add(1008, 1); // 烈日斩, 伤害提高4%
    // player.chSkillRecipe.add(1009, 1); // 烈日斩, 伤害提高5%
    // player.chSkillRecipe.add(1013, 1); // 烈日斩, 对原地静止的目标伤害提升10%

    // player.chSkillRecipe.add(1621, 1); // 生死劫, 伤害提高3%
    // player.chSkillRecipe.add(1622, 1); // 生死劫, 伤害提高4%
    // player.chSkillRecipe.add(1623, 1); // 生死劫, 伤害提高5%

    // player.chSkillRecipe.add(1019, 1); // 净世破魔击, 会心提高5%
    // player.chSkillRecipe.add(1015, 1); // 净世破魔击, 伤害提高4%
    // player.chSkillRecipe.add(1016, 1); // 净世破魔击, 伤害提高5%
    // player.chSkillRecipe.add(5206, 1); // 焚影圣诀心法下净世破魔击·月命中后回复20点月魂

    // player.chSkillRecipe.add(989, 1); // 幽月轮, 会心提高4%
    // player.chSkillRecipe.add(990, 1); // 幽月轮, 会心提高5%
    // player.chSkillRecipe.add(984, 1); // 幽月轮, 伤害提高3%
    // player.chSkillRecipe.add(985, 1); // 幽月轮, 伤害提高4%

    // player.chSkillRecipe.add(992, 1); // 银月斩, 会心提高3%
    // player.chSkillRecipe.add(993, 1); // 银月斩, 会心提高4%
    // player.chSkillRecipe.add(994, 1); // 银月斩, 会心提高5%

    // player.chSkillRecipe.add(1029, 1); // 光明相, 调息时间减少10秒
    // player.chSkillRecipe.add(1030, 1); // 光明相, 调息时间减少10秒
    // player.chSkillRecipe.add(1031, 1); // 光明相, 调息时间减少10秒

    // player.chSkillRecipe.add(1055, 1); // 驱夜断愁, 会心提高4%
    // player.chSkillRecipe.add(1056, 1); // 驱夜断愁, 会心提高5%
    // player.chSkillRecipe.add(1052, 1); // 驱夜断愁, 伤害提高4%
    // player.chSkillRecipe.add(1053, 1); // 驱夜断愁, 伤害提高5%

    // // player.DeactiveSkill(10242);

    // player.chAttr.atHasteBase = 95;
    // player.chAttr.atSpunkBase = 6380;

    // player.chAttr.atPhysicsAttackPowerBase = 6;
    // player.chAttr.atSolarAttackPowerBase = 19308;
    // player.chAttr.atLunarAttackPowerBase = 19308;
    // player.chAttr.atNeutralAttackPowerBase = 12721;
    // player.chAttr.atPoisonAttackPowerBase = 12721;

    // player.chAttr.atPhysicsCriticalStrike = 8325;
    // player.chAttr.atSolarCriticalStrike = 15048;
    // player.chAttr.atLunarCriticalStrike = 15048;
    // player.chAttr.atNeutralCriticalStrike = 8715;
    // player.chAttr.atPoisonCriticalStrike = 8715;

    // player.chAttr.atPhysicsCriticalDamagePowerBase = 0;
    // player.chAttr.atSolarCriticalDamagePowerBase = 188;
    // player.chAttr.atLunarCriticalDamagePowerBase = 188;
    // player.chAttr.atNeutralCriticalDamagePowerBase = 0;
    // player.chAttr.atPoisonCriticalDamagePowerBase = 0;

    // player.chAttr.atPhysicsOvercomeBase = 12;
    // player.chAttr.atMagicOvercome = 20360;

    // player.chAttr.atStrainBase = 21855;
    // player.chAttr.atSurplusValueBase = 9536;
    // player.chAttr.atMeleeWeaponDamageBase = 240;
    // player.chAttr.atMeleeWeaponDamageRand = 160;

    // npc.nLevel = 124;
    // npc.chAttr.atLevel = 124;
    // npc.chAttr.atPhysicsShieldBase = 27550;
    // npc.chAttr.atMagicShield = 27550;
    // npc.fMaxLife64 = 1e+10;
    // npc.fCurrentLife64 = 1e+10;

    auto start = std::chrono::steady_clock::now();

    // player.cast(3974);
    // player.vCheckSunMoonPower();

    // while (ns_frame::Event::run())
    //     ;
    // std::cout << ns_frame::Event::now() << std::endl;

    int fighttime = 300;

    callbackCastSkill(&player, nullptr);
    while (ns_frame::Event::now() < 1024 * fighttime) {
        ns_frame::Event::run();
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

    std::cout << "\n计算花费时间: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
    std::cout << "DPS: " << totalDamage / fighttime << std::endl;

    // for (auto &it : player.chDamage.damageList) {
    //     auto skill = ns_frame::SkillManager::get(it.skillID, it.skillLevel);
    //     /* FIXME: Replace this after proper implementation of damage source */
    //     ns_frame::DamageSource source = ns_frame::DamageSource::Skill;
    //     std::cout << (source == ns_frame::DamageSource::Skill ? ns_frame::UISkillManager::representDamage(it) : ns_frame::UIBuffManager::representDamage(it));
    // }

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

    macro.clear();
    ns_frame::LuaFunc::clear();

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