#ifndef CONCRETE_EFFECTS_BASE_H_
#define CONCRETE_EFFECTS_BASE_H_

#include "frame/character/character.h"

namespace ns_concrete {

class EffectBase {
public:
    EffectBase()          = default;
    virtual ~EffectBase() = default;

    virtual void active(ns_frame::Character *obj) const = 0;
};

} // namespace ns_concrete

#endif // CONCRETE_EFFECTS_BASE_H_
