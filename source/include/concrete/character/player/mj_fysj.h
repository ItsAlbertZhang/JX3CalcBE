#ifndef FRAME_CONCRETE_CHARACTER_PLAYER_MJFYSJ_H_
#define FRAME_CONCRETE_CHARACTER_PLAYER_MJFYSJ_H_

#include "frame/character/derived/player.h"

namespace ns_concrete {

class ChPlyMjFysj : public ns_frame::Player {
public:
    ChPlyMjFysj();
    virtual void macroPrepareDefault() override;
    virtual void macroRuntimeDefault() override;
    bool macroSwitchedOnce = false;
    void macroDefault0();
    void macroDefault1();
    void macroDefault2();
};

} // namespace ns_concrete

#endif