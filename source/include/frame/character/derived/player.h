#ifndef FRAME_CHARACTER_DERIVED_PLAYER_H_
#define FRAME_CHARACTER_DERIVED_PLAYER_H_

#include "frame/character/character.h"
#include <sol/sol.hpp>

namespace ns_frame {

class CustomLua {
public:
    static inline thread_local std::unordered_map<std::string, std::shared_ptr<CustomLua>> mapCustomLua;

    static std::shared_ptr<CustomLua> getCustomLua(const std::string &script) {
        if (!mapCustomLua.contains(script)) {
            mapCustomLua.emplace(script, std::make_shared<CustomLua>(script));
        }
        return mapCustomLua.at(script);
    }
    static void cancle(const std::string &script) {
        if (mapCustomLua.contains(script)) {
            mapCustomLua.erase(script);
        }
    }

    CustomLua(const std::string &script);

    sol::state                           lua; // lua 状态机
    sol::protected_function              macroPrepare;
    std::vector<sol::protected_function> macroRuntime;
};

class Player : public Character {
public:
    Player(int delayNetwork, int delayKeyboard);
    virtual void macroPrepareDefault() {}
    virtual void macroRuntimeDefault() {}
    virtual int  normalAttack() {
        return 1024; // 返回普通攻击间隔
    }

    int publicCooldownID = 0;
    int delayBase        = 0;
    int delayRand        = 0;
    int delayCustom      = 0;
    int macroIdx         = 0;

    std::shared_ptr<CustomLua> customLua;

    void macroRun();
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_DERIVED_PLAYER_H_
