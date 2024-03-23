#ifndef CONCRETE_CHARACTER_PLAYER_MJFYSJ_H_
#define CONCRETE_CHARACTER_PLAYER_MJFYSJ_H_

#include "frame/character/derived/player.h"

namespace jx3calc {
namespace concrete {

class MjFysj : public frame::Player {
public:
    MjFysj(int delayNetwork, int delayKeyboard);
    virtual void prepare() override;
    virtual int  normalAttack() override;
    virtual void macroDefault() override;

    bool macroSwitchedOnce = false;
    bool highPing          = false;
    void macroDefault0();
    void macroDefault1();
    void macroDefault2();
    void macroDefault3();
};

} // namespace concrete
} // namespace jx3calc

#endif // CONCRETE_CHARACTER_PLAYER_MJFYSJ_H_
