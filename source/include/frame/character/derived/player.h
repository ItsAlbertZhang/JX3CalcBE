#pragma once

#include "frame/character/character.h"
#include "frame/custom.h"

namespace jx3calc {
namespace frame {

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
