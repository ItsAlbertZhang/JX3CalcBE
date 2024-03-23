#ifndef CONCRETE_EFFECTS_BASE_H_
#define CONCRETE_EFFECTS_BASE_H_

#include "frame/character/character.h"

namespace jx3calc {
namespace concrete {

class EffectBase {
public:
    virtual void active(frame::Character *obj) const = 0;
};

} // namespace concrete
} // namespace jx3calc

#endif // CONCRETE_EFFECTS_BASE_H_
