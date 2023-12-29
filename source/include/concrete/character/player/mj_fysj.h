#ifndef CONCRETE_CHARACTER_PLAYER_MJFYSJ_H_
#define CONCRETE_CHARACTER_PLAYER_MJFYSJ_H_

#include "frame/character/derived/player.h"

namespace ns_concrete {

class MjFysj : public ns_frame::Player {
public:
    MjFysj(int delayNetwork, int delayKeybord);
    virtual void macroPrepareDefault() override;
    virtual void macroRuntimeDefault() override;
    virtual int  normalAttack() override;

    bool macroSwitchedOnce = false;
    void macroDefault0();
    void macroDefault1();
    void macroDefault2();
};

} // namespace ns_concrete

#endif // CONCRETE_CHARACTER_PLAYER_MJFYSJ_H_
