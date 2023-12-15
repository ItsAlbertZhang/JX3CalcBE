#ifndef FRAME_CHARACTER_DERIVED_PLAYER_H_
#define FRAME_CHARACTER_DERIVED_PLAYER_H_

#include "frame/character/character.h"
#include <filesystem>
#include <sol2/sol.hpp>
#include <string>

namespace ns_frame {

class MacroCustom {
public:
    MacroCustom(const std::string &script);
    MacroCustom(const std::filesystem::path &scriptfile);

    sol::state                           lua; // lua 状态机
    sol::protected_function              attrInit;
    sol::protected_function              macroPrepare;
    std::vector<sol::protected_function> macroRuntime;
};

class Player : public Character {
public:
    Player(int delayNetwork, int delayKeybord);
    virtual void macroPrepareDefault() {}
    virtual void macroRuntimeDefault() {}
    virtual int  normalAttack() {
        return 1024; // 返回普通攻击间隔
    }

    int          publicCooldownID = 0;
    int          delayBase        = 0;
    int          delayRand        = 0;
    int          delayCustom      = 0;
    int          macroIdx         = 0;
    MacroCustom *macroCustom      = nullptr;

    void macroRun();
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_DERIVED_PLAYER_H_
