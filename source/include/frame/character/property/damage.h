#ifndef FRAME_CHARACTER_PROPERTY_DAMAGE_H_
#define FRAME_CHARACTER_PROPERTY_DAMAGE_H_

#include <vector>

namespace ns_frame {

using event_tick_t = unsigned long long;

enum class DamageType {
    Physics,
    Solar,
    Lunar,
    Neutral,
    Poison,
    COUNT,
};

enum class DamageSource {
    skill,
    buff,
    COUNT,
};

class Damage {
public:
    Damage(
        event_tick_t tick,
        DamageSource source,
        int          id,
        int          level,
        bool         isCritical,
        int          damage,
        DamageType   damageType
    )
        : tick(tick), source(source), id(id), level(level),
          isCritical(isCritical), damage(damage), damageType(damageType) {}

    event_tick_t tick;
    DamageSource source;
    int          id;
    int          level;
    bool         isCritical;
    int          damage;
    DamageType   damageType;
};

class ChDamage {
public:
    std::vector<Damage> damageList;
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_PROPERTY_DAMAGE_H_
