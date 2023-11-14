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
    event_tick_t tick           = 0;
    int          damageType     = 0;
    int          id             = 0;
    int          level          = 0;
    int          damageBase     = 0;
    int          damageCritical = 0;
    int          damageExcept   = 0;
    int          criticalRate   = 0;
    bool         isCritical     = false;
    bool         isBuff         = false;

    Damage &operator+=(const Damage &other) {
        this->tick       = other.tick;
        this->damageType = this->damageType | other.damageType; // 按位或
        this->id         = other.id;
        this->level      = other.level;
        this->damageBase += other.damageBase;
        this->damageCritical += other.damageCritical;
        this->damageExcept += other.damageExcept;
        this->criticalRate = other.criticalRate;
        this->isCritical   = other.isCritical;
        this->isBuff       = other.isBuff;
        return *this;
    }
};

// class ChDamage {
// public:
//     std::vector<Damage> damageList;
// };
using ChDamage = std::vector<Damage>;

} // namespace frame
} // namespace jx3calc
