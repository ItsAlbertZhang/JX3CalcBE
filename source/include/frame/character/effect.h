#pragma once

#include "frame/character/character.h"

namespace jx3calc {
namespace frame {

class Effect {
public:
    virtual void active(frame::Character *obj) = 0;
};

} // namespace frame
} // namespace jx3calc
