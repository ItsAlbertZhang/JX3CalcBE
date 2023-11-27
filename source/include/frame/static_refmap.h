#ifndef FRAME_STATIC_REFMAP_H_
#define FRAME_STATIC_REFMAP_H_

#include <string>
#include <unordered_map>

namespace ns_framestatic {

enum class enumLuaAttributeType {
    CALL_LUNAR_DAMAGE,
    CALL_NEUTRAL_DAMAGE,
    CALL_PHYSICS_DAMAGE,
    CALL_POISON_DAMAGE,
    CALL_SOLAR_DAMAGE,
    CAST_SKILL,
    CAST_SKILL_TARGET_DST,
    CURRENT_MOON_ENERGY,
    CURRENT_SUN_ENERGY,
    DASH,
    EXECUTE_SCRIPT,
    EXECUTE_SCRIPT_WITH_PARAM,
    LUNAR_ATTACK_POWER_PERCENT,
    NEUTRAL_ATTACK_POWER_PERCENT,
    PHYSICS_ATTACK_POWER_PERCENT,
    POISON_ATTACK_POWER_PERCENT,
    SKILL_LUNAR_DAMAGE,
    SKILL_LUNAR_DAMAGE_RAND,
    SKILL_NEUTRAL_DAMAGE,
    SKILL_NEUTRAL_DAMAGE_RAND,
    SKILL_PHYSICS_DAMAGE,
    SKILL_PHYSICS_DAMAGE_RAND,
    SKILL_POISON_DAMAGE,
    SKILL_POISON_DAMAGE_RAND,
    SKILL_SOLAR_DAMAGE,
    SKILL_SOLAR_DAMAGE_RAND,
    SOLAR_ATTACK_POWER_PERCENT,
    COUNT,
};
inline const char *refLuaAttributeType[] = {
    "CALL_LUNAR_DAMAGE",
    "CALL_NEUTRAL_DAMAGE",
    "CALL_PHYSICS_DAMAGE",
    "CALL_POISON_DAMAGE",
    "CALL_SOLAR_DAMAGE",
    "CAST_SKILL",
    "CAST_SKILL_TARGET_DST",
    "CURRENT_MOON_ENERGY",
    "CURRENT_SUN_ENERGY",
    "DASH",
    "EXECUTE_SCRIPT",
    "EXECUTE_SCRIPT_WITH_PARAM",
    "LUNAR_ATTACK_POWER_PERCENT",
    "NEUTRAL_ATTACK_POWER_PERCENT",
    "PHYSICS_ATTACK_POWER_PERCENT",
    "POISON_ATTACK_POWER_PERCENT",
    "SKILL_LUNAR_DAMAGE",
    "SKILL_LUNAR_DAMAGE_RAND",
    "SKILL_NEUTRAL_DAMAGE",
    "SKILL_NEUTRAL_DAMAGE_RAND",
    "SKILL_PHYSICS_DAMAGE",
    "SKILL_PHYSICS_DAMAGE_RAND",
    "SKILL_POISON_DAMAGE",
    "SKILL_POISON_DAMAGE_RAND",
    "SKILL_SOLAR_DAMAGE",
    "SKILL_SOLAR_DAMAGE_RAND",
    "SOLAR_ATTACK_POWER_PERCENT",
};

enum class enumTabAttribute {
    atAllDamageAddPercent,
    atLunarDamageCoefficient,
    atSolarDamageCoefficient,
    COUNT,
};
inline const std::string refTabAttribute[] = {
    "atAllDamageAddPercent",
    "atLunarDamageCoefficient",
    "atSolarDamageCoefficient",
};
inline const std::unordered_map<std::string, enumTabAttribute> mapTabAttribute = {
    {"atAllDamageAddPercent", enumTabAttribute::atAllDamageAddPercent},
    {"atLunarDamageCoefficient", enumTabAttribute::atLunarDamageCoefficient},
    {"atSolarDamageCoefficient", enumTabAttribute::atSolarDamageCoefficient},
};

} // namespace ns_framestatic

#endif // FRAME_STATIC_REFMAP_H_
