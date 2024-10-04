#pragma once

#include "frame/ref/ref.h"

namespace jx3calc {
namespace frame {

enum class ref::lua::ATTRIBUTE_TYPE {
    ACCUMULATE, // 禅那
    ACTIVE_THREAT_COEFFICIENT,
    ADD_DAMAGE_BY_DST_MOVE_STATE,
    ALL_DAMAGE_ADD_PERCENT,
    BEAT_BACK_RATE,
    CALL_BUFF,
    CALL_ADAPTIVE_DAMAGE,
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
    CURRENT_ENERGY, // 神机值
    CURRENT_MOON_ENERGY,
    CURRENT_SUN_ENERGY,
    DASH,
    DECRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE,
    DEL_MULTI_GROUP_BUFF_BY_FUNCTIONTYPE,
    DEL_MULTI_GROUP_BUFF_BY_ID,
    DEL_SINGLE_BUFF_BY_ID_AND_LEVEL,
    DO_ACTION,
    DROP_DEFENCE,
    DST_NPC_DAMAGE_COEFFICIENT,
    EXECUTE_SCRIPT,
    EXECUTE_SCRIPT_WITH_PARAM,
    GLOBAL_DAMGAGE_FACTOR,
    KUNGFU_TYPE,
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
    MOON_POWER_VALUE,
    NEUTRAL_ATTACK_POWER_PERCENT,
    NEUTRAL_CRITICAL_STRIKE_BASE_RATE,
    PHYSICS_ATTACK_POWER_PERCENT,
    PHYSICS_CRITICAL_STRIKE_BASE_RATE,
    PHYSICS_SHIELD_BASE,
    POISON_ATTACK_POWER_PERCENT,
    POISON_CRITICAL_STRIKE_BASE_RATE,
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
    STOP,
    SUN_POWER_VALUE,
    COUNT,
};

} // namespace frame
} // namespace jx3calc
