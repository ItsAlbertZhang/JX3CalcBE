#ifndef FRAME_CHARACTER_PROPERTY_DAMAGE_H_
#define FRAME_CHARACTER_PROPERTY_DAMAGE_H_

#include "frame/common/event.h"
#include <vector>

namespace ns_frame {

enum class DamageType {
    Physics,
    Solar,
    Lunar,
    Neutral,
    Poison,
    COUNT,
};

class Damage {
public:
    event_tick_t tick;
    DamageType   damageType;
    int          id;
    int          level;
    int          damageBase;
    int          damageCritical;
    int          damageExcept;
    int          criticalRate;
    bool         isCritical;
    bool         isBuff;
};

// class ChDamage {
// public:
//     std::vector<Damage> damageList;
// };
using ChDamage = std::vector<Damage>;

} // namespace ns_frame

#endif // FRAME_CHARACTER_PROPERTY_DAMAGE_H_
