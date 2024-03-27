#pragma once

#include "frame/character/character.h"
#include <sol/sol.hpp>

namespace jx3calc {
namespace frame {

class CustomLua {
public:
    static inline thread_local std::unordered_map<std::string, std::shared_ptr<CustomLua>> mapCustomLua;

    static std::shared_ptr<CustomLua> get(const std::string &script);
    static void                       cancel(const std::string &script);
    static std::string                parse(std::vector<std::string> macroList);

    CustomLua(const std::string &script);

    sol::state                           lua; // lua 状态机
    sol::protected_function              init;
    std::vector<sol::protected_function> macroRuntime;
};

class Player : public Character {
public:
    Player(int delayNetwork, int delayKeyboard);
    virtual void prepare() {}
    virtual int  normalAttack() {
        return 1024; // 返回普通攻击间隔
    }
    virtual void macroDefault() {}

    int publicCooldownID = 0;
    int delayBase        = 0;
    int delayRand        = 0;
    int delayCustom      = 0;
    int macroIdx         = 0;

    std::shared_ptr<CustomLua> customLua;

    void macroRun();
};

} // namespace frame
} // namespace jx3calc
