#ifndef FRAME_CONCRETE_CHARACTER_PLAYER_MJFYSJ_H_
#define FRAME_CONCRETE_CHARACTER_PLAYER_MJFYSJ_H_

#include "frame/character/derived/player.h"

namespace ns_concrete {

class MjFysj : public ns_frame::Player {
public:
    MjFysj();
    virtual void macroPrepareDefault() override;
    virtual void macroRuntimeDefault() override;

    bool macroSwitchedOnce = false;
    void macroDefault0();
    void macroDefault1();
    void macroDefault2();
};

} // namespace ns_concrete

#endif // FRAME_CONCRETE_CHARACTER_PLAYER_MJFYSJ_H_