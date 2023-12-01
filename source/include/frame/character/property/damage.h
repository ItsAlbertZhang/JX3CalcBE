#ifndef FRAME_CHARACTER_PROPERTY_DAMAGE_H_
#define FRAME_CHARACTER_PROPERTY_DAMAGE_H_

#include <string>
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

class Damage {
public:
    Damage(event_tick_t tick, int skillID, int skillLevel, bool isCritical, int damage, DamageType damageType)
        : tick(tick), skillID(skillID), skillLevel(skillLevel), isCritical(isCritical), damage(damage), damageType(damageType) {}
    event_tick_t tick;
    int skillID;
    int skillLevel;
    bool isCritical;
    int damage;
    DamageType damageType;
};

class CharacterDamage {
public:
    std::vector<Damage> damageList;
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_PROPERTY_DAMAGE_H_