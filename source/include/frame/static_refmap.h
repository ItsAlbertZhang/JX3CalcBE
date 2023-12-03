#ifndef FRAME_STATIC_REFMAP_H_
#define FRAME_STATIC_REFMAP_H_

#include <string>
#include <unordered_map>

namespace ns_framestatic {

enum class enumLuaAttributeType {
    ACTIVE_THREAT_COEFFICIENT,
    BEAT_BACK_RATE,
    CALL_BUFF,
    CALL_LUNAR_DAMAGE,
    CALL_NEUTRAL_DAMAGE,
    CALL_PHYSICS_DAMAGE,
    CALL_POISON_DAMAGE,
    CALL_SOLAR_DAMAGE,
    CALL_SURPLUS_LUNAR_DAMAGE,
    CALL_SURPLUS_NEUTRAL_DAMAGE,
    CALL_SURPLUS_PHYSICS_DAMAGE,
    CALL_SURPLUS_POISON_DAMAGE,
    CALL_SURPLUS_SOLAR_DAMAGE,
    CAST_SKILL,
    CAST_SKILL_TARGET_DST,
    CURRENT_MOON_ENERGY,
    CURRENT_SUN_ENERGY,
    DASH,
    DECRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE,
    DEL_MULTI_GROUP_BUFF_BY_FUNCTIONTYPE,
    DEL_SINGLE_BUFF_BY_ID_AND_LEVEL,
    DROP_DEFENCE,
    DST_NPC_DAMAGE_COEFFICIENT,
    EXECUTE_SCRIPT,
    EXECUTE_SCRIPT_WITH_PARAM,
    GLOBAL_DAMGAGE_FACTOR,
    LUNAR_ATTACK_POWER_BASE,
    LUNAR_ATTACK_POWER_PERCENT,
    LUNAR_CRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE,
    LUNAR_CRITICAL_STRIKE_BASE_RATE,
    MAGIC_CRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE,
    MAGIC_SHIELD,
    MANA_REPLENISH_PERCENT,
    MAX_LIFE_PERCENT_ADD,
    MAX_MOON_ENERGY,
    MAX_SUN_ENERGY,
    NEUTRAL_ATTACK_POWER_PERCENT,
    PHYSICS_ATTACK_POWER_PERCENT,
    PHYSICS_SHIELD_BASE,
    POISON_ATTACK_POWER_PERCENT,
    SET_ADAPTIVE_SKILL_TYPE,
    SET_TALENT_RECIPE,
    SKILL_EVENT_HANDLER,
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
    SOLAR_ATTACK_POWER_BASE,
    SOLAR_ATTACK_POWER_PERCENT,
    SOLAR_CRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE,
    SOLAR_CRITICAL_STRIKE_BASE_RATE,
    SPUNK_TO_SOLAR_AND_LUNAR_ATTACK_POWER_COF,
    SPUNK_TO_SOLAR_AND_LUNAR_CRITICAL_STRIKE_COF,
    COUNT,
};
inline const char *refLuaAttributeType[] = {
    "ACTIVE_THREAT_COEFFICIENT",
    "BEAT_BACK_RATE",
    "CALL_BUFF",
    "CALL_LUNAR_DAMAGE",
    "CALL_NEUTRAL_DAMAGE",
    "CALL_PHYSICS_DAMAGE",
    "CALL_POISON_DAMAGE",
    "CALL_SOLAR_DAMAGE",
    "CALL_SURPLUS_LUNAR_DAMAGE",
    "CALL_SURPLUS_NEUTRAL_DAMAGE",
    "CALL_SURPLUS_PHYSICS_DAMAGE",
    "CALL_SURPLUS_POISON_DAMAGE",
    "CALL_SURPLUS_SOLAR_DAMAGE",
    "CAST_SKILL",
    "CAST_SKILL_TARGET_DST",
    "CURRENT_MOON_ENERGY",
    "CURRENT_SUN_ENERGY",
    "DASH",
    "DECRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE",
    "DEL_MULTI_GROUP_BUFF_BY_FUNCTIONTYPE",
    "DEL_SINGLE_BUFF_BY_ID_AND_LEVEL",
    "DROP_DEFENCE",
    "DST_NPC_DAMAGE_COEFFICIENT",
    "EXECUTE_SCRIPT",
    "EXECUTE_SCRIPT_WITH_PARAM",
    "GLOBAL_DAMGAGE_FACTOR",
    "LUNAR_ATTACK_POWER_BASE",
    "LUNAR_ATTACK_POWER_PERCENT",
    "LUNAR_CRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE",
    "LUNAR_CRITICAL_STRIKE_BASE_RATE",
    "MAGIC_CRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE",
    "MAGIC_SHIELD",
    "MANA_REPLENISH_PERCENT",
    "MAX_LIFE_PERCENT_ADD",
    "MAX_MOON_ENERGY",
    "MAX_SUN_ENERGY",
    "NEUTRAL_ATTACK_POWER_PERCENT",
    "PHYSICS_ATTACK_POWER_PERCENT",
    "PHYSICS_SHIELD_BASE",
    "POISON_ATTACK_POWER_PERCENT",
    "SET_ADAPTIVE_SKILL_TYPE",
    "SET_TALENT_RECIPE",
    "SKILL_EVENT_HANDLER",
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
    "SOLAR_ATTACK_POWER_BASE",
    "SOLAR_ATTACK_POWER_PERCENT",
    "SOLAR_CRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE",
    "SOLAR_CRITICAL_STRIKE_BASE_RATE",
    "SPUNK_TO_SOLAR_AND_LUNAR_ATTACK_POWER_COF",
    "SPUNK_TO_SOLAR_AND_LUNAR_CRITICAL_STRIKE_COF",
};

enum class enumTabAttribute {
    atActiveThreatCoefficient,
    atAddTransparencyValue,
    atAllDamageAddPercent,
    atAllMagicDamageAddPercent,
    atAllShieldIgnorePercent,
    atBeImmunisedStealthEnable,
    atCallLunarDamage,
    atExecuteScript,
    atHalt,
    atImmuneSkillMove,
    atImmunity,
    atKnockedDownRate,
    atLunarCriticalStrikeBaseRate,
    atLunarDamageCoefficient,
    atMagicCriticalDamagePowerBaseKiloNumRate,
    atMoveSpeedPercent,
    atNoLimitChangeSkillIcon,
    atSetSelectableType,
    atSetTalentRecipe,
    atSkillEventHandler,
    atSolarCriticalStrikeBaseRate,
    atSolarDamageCoefficient,
    atStealth,
    COUNT,
};
inline const std::string refTabAttribute[] = {
    "atActiveThreatCoefficient",
    "atAddTransparencyValue",
    "atAllDamageAddPercent",
    "atAllMagicDamageAddPercent",
    "atAllShieldIgnorePercent",
    "atBeImmunisedStealthEnable",
    "atCallLunarDamage",
    "atExecuteScript",
    "atHalt",
    "atImmuneSkillMove",
    "atImmunity",
    "atKnockedDownRate",
    "atLunarCriticalStrikeBaseRate",
    "atLunarDamageCoefficient",
    "atMagicCriticalDamagePowerBaseKiloNumRate",
    "atMoveSpeedPercent",
    "atNoLimitChangeSkillIcon",
    "atSetSelectableType",
    "atSetTalentRecipe",
    "atSkillEventHandler",
    "atSolarCriticalStrikeBaseRate",
    "atSolarDamageCoefficient",
    "atStealth",
};
inline const std::unordered_map<std::string, enumTabAttribute> mapTabAttribute = {
    {"atActiveThreatCoefficient", enumTabAttribute::atActiveThreatCoefficient},
    {"atAddTransparencyValue", enumTabAttribute::atAddTransparencyValue},
    {"atAllDamageAddPercent", enumTabAttribute::atAllDamageAddPercent},
    {"atAllMagicDamageAddPercent", enumTabAttribute::atAllMagicDamageAddPercent},
    {"atAllShieldIgnorePercent", enumTabAttribute::atAllShieldIgnorePercent},
    {"atBeImmunisedStealthEnable", enumTabAttribute::atBeImmunisedStealthEnable},
    {"atCallLunarDamage", enumTabAttribute::atCallLunarDamage},
    {"atExecuteScript", enumTabAttribute::atExecuteScript},
    {"atHalt", enumTabAttribute::atHalt},
    {"atImmuneSkillMove", enumTabAttribute::atImmuneSkillMove},
    {"atImmunity", enumTabAttribute::atImmunity},
    {"atKnockedDownRate", enumTabAttribute::atKnockedDownRate},
    {"atLunarCriticalStrikeBaseRate", enumTabAttribute::atLunarCriticalStrikeBaseRate},
    {"atLunarDamageCoefficient", enumTabAttribute::atLunarDamageCoefficient},
    {"atMagicCriticalDamagePowerBaseKiloNumRate", enumTabAttribute::atMagicCriticalDamagePowerBaseKiloNumRate},
    {"atMoveSpeedPercent", enumTabAttribute::atMoveSpeedPercent},
    {"atNoLimitChangeSkillIcon", enumTabAttribute::atNoLimitChangeSkillIcon},
    {"atSetSelectableType", enumTabAttribute::atSetSelectableType},
    {"atSetTalentRecipe", enumTabAttribute::atSetTalentRecipe},
    {"atSkillEventHandler", enumTabAttribute::atSkillEventHandler},
    {"atSolarCriticalStrikeBaseRate", enumTabAttribute::atSolarCriticalStrikeBaseRate},
    {"atSolarDamageCoefficient", enumTabAttribute::atSolarDamageCoefficient},
    {"atStealth", enumTabAttribute::atStealth},
};

} // namespace ns_framestatic

#endif // FRAME_STATIC_REFMAP_H_