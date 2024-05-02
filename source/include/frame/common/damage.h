#pragma once

#include "frame/common/event.h"
#include <vector>

namespace jx3calc {
namespace frame {

enum class DamageType {
    Physics,
    Solar,
    Neutral,
    Lunar,
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

} // namespace frame
} // namespace jx3calc
