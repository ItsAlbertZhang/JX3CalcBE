#ifndef FRAME_CHARACTER_DERIVED_PLAYER_H_
#define FRAME_CHARACTER_DERIVED_PLAYER_H_

#include "frame/character/character.h"
#include "frame/custom/base.h"
#include "frame/custom/lua.h"
namespace ns_frame {

class Player : public Character {
public:
    Player(int delayNetwork, int delayKeyboard);
    virtual void macroPrepareDefault() {}
    virtual void macroRuntimeDefault() {}
    virtual int  normalAttack() {
        return 1024; // 返回普通攻击间隔
    }

    int        publicCooldownID = 0;
    int        delayBase        = 0;
    int        delayRand        = 0;
    int        delayCustom      = 0;
    int        macroIdx         = 0;
    enumCustom customType       = enumCustom::none;
    CustomLua *customLua        = nullptr;

    void macroRun();
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_DERIVED_PLAYER_H_
