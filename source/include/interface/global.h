#ifndef MAIN_INTERFACE_FUNC_H_
#define MAIN_INTERFACE_FUNC_H_

#include <string>

namespace interface {

// lua 中的全局函数. 注意, 这是一个静态类, 无法创建实例.
class GlobalFunction {
public:
    static void Include(const std::string &filename);
};

class GlobalEnum {
public:
    static const char *AttributeEffectMode[];
    static const char *AttributeType[];
    static const char *BuffCompareFlag[];
    enum class ATTRIBUTE_EFFECT_MODE {
        EFFECT_TO_SELF_NOT_ROLLBACK,
        EFFECT_TO_SELF_AND_ROLLBACK,
        EFFECT_TO_DEST_NOT_ROLLBACK,
        EFFECT_TO_DEST_AND_ROLLBACK,
        COUNT,
    };
    enum class ATTRIBUTE_TYPE {
        RUN_SPEED_BASE,
        MOVE_SPEED_PERCENT,
        MIN_RUN_SPEED,
        MAX_RUN_SPEED,
        MAX_JUMP_COUNT,
        SET_JUMP_COUNT,
        FLY_FLAG,
        ON_TOWER_FLAG,
        ON_PARACHUTE_FLAG,
        HORSE_ATTRIBUTE,
        HORSE_RUN_TYPE,
        HEIGHT,
        TRANSFORM,
        GRAVITY_BASE,
        GRAVITY_PERCENT,
        JUMP_SPEED_BASE,
        JUMP_SPEED_PERCENT,
        HORSE_JUMP_SPEED_ADDITIONAL,
        DROP_DEFENCE,
        DIVING_FRAME_BASE,
        DIVING_FRAME_PERCENT,
        WATER_FLY_ABILITY,
        CAN_NOT_JUMP,
        STRENGTH_BASE,
        STRENGTH_BASE_PERCENT_ADD,
        AGILITY_BASE,
        AGILITY_BASE_PERCENT_ADD,
        VITALITY_BASE,
        VITALITY_BASE_PERCENT_ADD,
        SPIRIT_BASE,
        SPIRIT_BASE_PERCENT_ADD,
        SPUNK_BASE,
        SPUNK_BASE_PERCENT_ADD,
        HASTE_BASE,
        HASTE_BASE_PERCENT_ADD,
        UNLIMIT_HASTE_BASE_PERCENT_ADD,
        TRANSMIT_TRAIN,
        MAX_TRAIN_VALUE,
        BASE_VENATION_COF,
        PHYSICS_VENATION_COF,
        MAGIC_VENATION_COF,
        ASSIST_VENATION_COF,
        BASE_POTENTIAL_ADD,
        CURRENT_LIFE,
        MAX_LIFE_BASE,
        MAX_LIFE_PERCENT_ADD,
        MAX_LIFE_ADDITIONAL,
        FINAL_MAX_LIFE_ADD_PERCENT,
        LIFE_REPLENISH,
        LIFE_REPLENISH_PERCENT,
        LIFE_REPLENISH_COEFFICIENT,
        LIFE_REPLENISH_EXT,
        CURRENT_MANA,
        MAX_MANA_BASE,
        MAX_MANA_PERCENT_ADD,
        MAX_MANA_ADDITIONAL,
        MANA_PERCENT_ADD,
        MANA_REPLENISH,
        MANA_REPLENISH_PERCENT,
        MANA_REPLENISH_COEFFICIENT,
        MANA_REPLENISH_EXT,
        CURRENT_RAGE,
        MAX_RAGE,
        RAGE_REPLENISH,
        CURRENT_ENERGY,
        MAX_ENERGY,
        ENERGY_REPLENISH,
        SUN_POWER_VALUE,
        MOON_POWER_VALUE,
        CURRENT_SUN_ENERGY,
        MAX_SUN_ENERGY,
        SUN_ENERGY_ADD_PERCENT,
        SUN_ENERGY_REPLENISH,
        SUN_ENERGY_FALL_OFF,
        STOP_MAKE_SUN_POWER,
        CURRENT_MOON_ENERGY,
        MAX_MOON_ENERGY,
        MOON_ENERGY_ADD_PERCENT,
        MOON_ENERGY_REPLENISH,
        MOON_ENERGY_FALL_OFF,
        CURRENT_STAMINA,
        MAX_STAMINA,
        CURRENT_THEW,
        MAX_THEW,
        ACTIVE_THREAT_COEFFICIENT,
        PASSIVE_THREAT_COEFFICIENT,
        DODGE,
        DODGE_BASE_RATE,
        PARRY_BASE_RATE,
        PARRY_BASE,
        PARRY_PERCENT,
        PARRYVALUE_BASE,
        PARRYVALUE_PERCENT,
        PARRY_VALUE_TO_MAGIC_SHIELD_COF,
        SENSE,
        STRAIN_BASE,
        STRAIN_PERCENT,
        STRAIN_RATE,
        TOUGHNESS_BASE_RATE,
        TOUGHNESS_BASE,
        TOUGHNESS_PERCENT,
        DECRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE,
        DECRITICAL_DAMAGE_POWER_BASE,
        DECRITICAL_DAMAGE_POWER_PERCENT,
        POSITIVE_SHIELD,
        NEGATIVE_SHIELD,
        GLOBAL_RESIST_PERCENT,
        GLOBAL_BLOCK,
        GLOBAL_THERAPY_ABSORB,
        GLOBAL_THERAPY_ABSORB_BY_SELF_MAX_LIFE,
        GLOBAL_DAMAGE_ABSORB,
        GLOBAL_DAMAGE_ABSORB_BY_SELF_MAX_LIFE,
        GLOBAL_DAMAGE_ABSORB_BY_SELF_SPIRIT,
        GLOBAL_DAMAGE_ABSORB_BY_SELF_AGILITY,
        GLOBAL_DAMAGE_ABSORB_BY_SELF_VITALITY,
        GLOBAL_DAMAGE_MANA_SHIELD,
        DAMAGE_TO_LIFE_FOR_SELF,
        DAMAGE_TO_MANA_FOR_SELF,
        MAX_DAMAGE_TO_LIFE_VALUE,
        MAX_DAMAGE_TO_MANA_VALUE,
        BEAT_BACK_RATE,
        KNOCKED_BACK_RATE,
        KNOCKED_OFF_RATE,
        KNOCKED_DOWN_RATE,
        PULL_RATE,
        IMMUNE_SKILL_MOVED,
        REPULSED_RATE,
        IMMORTAL,
        MOUNTING_RATE,
        MELEE_WEAPON_DAMAGE_BASE,
        MELEE_WEAPON_DAMAGE_PERCENT,
        MELEE_WEAPON_DAMAGE_RAND,
        MELEE_WEAPON_DAMAGE_RAND_PERCENT,
        RANGE_WEAPON_DAMAGE_BASE,
        RANGE_WEAPON_DAMAGE_PERCENT,
        RANGE_WEAPON_DAMAGE_RAND,
        RANGE_WEAPON_DAMAGE_RAND_PERCENT,
        SKILL_PHYSICS_DAMAGE,
        SKILL_PHYSICS_DAMAGE_RAND,
        SKILL_PHYSICS_DAMAGE_PERCENT,
        PHYSICS_DAMAGE_COEFFICIENT,
        PHYSICS_ATTACK_POWER_BASE,
        PHYSICS_ATTACK_POWER_PERCENT,
        PHYSICS_HIT_BASE_RATE,
        PHYSICS_HIT_VALUE,
        PHYSICS_CRITICAL_STRIKE_BASE_RATE,
        PHYSICS_CRITICAL_STRIKE,
        PHYSICS_CRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE,
        PHYSICS_CRITICAL_DAMAGE_POWER_BASE,
        PHYSICS_CRITICAL_DAMAGE_POWER_PERCENT,
        PHYSICS_OVERCOME_BASE,
        PHYSICS_OVERCOME_PERCENT,
        PHYSICS_RESIST_PERCENT,
        PHYSICS_DEFENCE_ADD,
        PHYSICS_DEFENCE_MAX,
        PHYSICS_SHIELD_BASE,
        PHYSICS_SHIELD_PERCENT,
        PHYSICS_SHIELD_ADDITIONAL,
        PHYSICS_REFLECTION,
        PHYSICS_REFLECTION_PERCENT,
        PHYSICS_BLOCK,
        PHYSICS_DAMAGE_ABSORB,
        PHYSICS_DAMAGE_MANA_SHIELD,
        ALL_TYPE_HIT_VALUE,
        ALL_TYPE_CRITICAL_STRIKE,
        ALL_TYPE_CRITICAL_DAMAGE_POWER_BASE,
        SKILL_MAGIC_DAMAGE,
        MAGIC_ATTACK_POWER_BASE,
        MAGIC_ATTACK_POWER_PERCENT,
        MAGIC_CRITICAL_STRIKE,
        MAGIC_SHIELD,
        MAGIC_CRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE,
        MAGIC_CRITICAL_DAMAGE_POWER_BASE,
        MAGIC_CRITICAL_DAMAGE_POWER_PERCENT,
        MAGIC_OVERCOME,
        MAGIC_HIT_BASE_RATE,
        MAGIC_HIT_VALUE,
        SKILL_SOLAR_DAMAGE,
        SKILL_SOLAR_DAMAGE_RAND,
        SKILL_SOLAR_DAMAGE_PERCENT,
        SOLAR_DAMAGE_COEFFICIENT,
        SOLAR_ATTACK_POWER_BASE,
        SOLAR_ATTACK_POWER_PERCENT,
        SOLAR_HIT_BASE_RATE,
        SOLAR_HIT_VALUE,
        SOLAR_CRITICAL_STRIKE_BASE_RATE,
        SOLAR_CRITICAL_STRIKE,
        SOLAR_CRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE,
        SOLAR_CRITICAL_DAMAGE_POWER_BASE,
        SOLAR_CRITICAL_DAMAGE_POWER_PERCENT,
        SOLAR_OVERCOME_BASE,
        SOLAR_OVERCOME_PERCENT,
        SOLAR_MAGIC_RESIST_PERCENT,
        SOLAR_MAGIC_SHIELD_BASE,
        SOLAR_MAGIC_SHIELD_PERCENT,
        SOLAR_MAGIC_REFLECTION,
        SOLAR_MAGIC_REFLECTION_PERCENT,
        SOLAR_MAGIC_BLOCK,
        SOLAR_DAMAGE_ABSORB,
        SOLAR_DAMAGE_MANA_SHIELD,
        SKILL_NEUTRAL_DAMAGE,
        SKILL_NEUTRAL_DAMAGE_RAND,
        SKILL_NEUTRAL_DAMAGE_PERCENT,
        NEUTRAL_DAMAGE_COEFFICIENT,
        NEUTRAL_ATTACK_POWER_BASE,
        NEUTRAL_ATTACK_POWER_PERCNET,
        NEUTRAL_HIT_BASE_RATE,
        NEUTRAL_HIT_VALUE,
        NEUTRAL_CRITICAL_STRIKE_BASE_RATE,
        NEUTRAL_CRITICAL_STRIKE,
        NEUTRAL_CRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE,
        NEUTRAL_CRITICAL_DAMAGE_POWER_BASE,
        NEUTRAL_CRITICAL_DAMAGE_POWER_PERCENT,
        NEUTRAL_OVERCOME_BASE,
        NEUTRAL_OVERCOME_PERCENT,
        NEUTRAL_MAGIC_RESIST_PERCENT,
        NEUTRAL_MAGIC_SHIELD_BASE,
        NEUTRAL_MAGIC_SHIELD_PERCENT,
        NEUTRAL_MAGIC_REFLECTION,
        NEUTRAL_MAGIC_REFLECTION_PERCENT,
        NEUTRAL_MAGIC_BLOCK,
        NEUTRAL_DAMAGE_ABSORB,
        NEUTRAL_DAMAGE_MANA_SHIELD,
        SKILL_LUNAR_DAMAGE,
        SKILL_LUNAR_DAMAGE_RAND,
        SKILL_LUNAR_DAMAGE_PERCENT,
        LUNAR_DAMAGE_COEFFICIENT,
        LUNAR_ATTACK_POWER_BASE,
        LUNAR_ATTACK_POWER_PERCENT,
        LUNAR_HIT_BASE_RATE,
        LUNAR_HIT_VALUE,
        LUNAR_CRITICAL_STRIKE_BASE_RATE,
        LUNAR_CRITICAL_STRIKE,
        LUNAR_CRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE,
        LUNAR_CRITICAL_DAMAGE_POWER_BASE,
        LUNAR_CRITICAL_DAMAGE_POWER_PERCENT,
        LUNAR_OVERCOME_BASE,
        LUNAR_OVERCOME_PERCENT,
        LUNAR_MAGIC_RESIST_PERCENT,
        LUNAR_MAGIC_SHIELD_BASE,
        LUNAR_MAGIC_SHIELD_PERCENT,
        LUNAR_MAGIC_REFLECTION,
        LUNAR_MAGIC_REFLECTION_PERCENT,
        LUNAR_MAGIC_BLOCK,
        LUNAR_DAMAGE_ABSORB,
        LUNAR_DAMAGE_MANA_SHIELD,
        SKILL_POISON_DAMAGE,
        SKILL_POISON_DAMAGE_RAND,
        SKILL_POISON_DAMAGE_PERCENT,
        POISON_DAMAGE_COEFFICIENT,
        POISON_ATTACK_POWER_BASE,
        POISON_ATTACK_POWER_PERCENT,
        POISON_HIT_BASE_RATE,
        POISON_HIT_VALUE,
        POISON_CRITICAL_STRIKE_BASE_RATE,
        POISON_CRITICAL_STRIKE,
        POISON_CRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE,
        POISON_CRITICAL_DAMAGE_POWER_BASE,
        POISON_CRITICAL_DAMAGE_POWER_PERCENT,
        POISON_OVERCOME_BASE,
        POISON_OVERCOME_PERCENT,
        POISON_MAGIC_RESIST_PERCENT,
        POISON_MAGIC_SHIELD_BASE,
        POISON_MAGIC_SHIELD_PERCENT,
        POISON_MAGIC_REFLECTION,
        POISON_MAGIC_REFLECTION_PERCENT,
        POISON_MAGIC_BLOCK,
        POISON_DAMAGE_ABSORB,
        POISON_DAMAGE_MANA_SHIELD,
        THERAPY_POWER_BASE,
        THERAPY_POWER_PERCENT,
        SKILL_THERAPY,
        SKILL_THERAPY_RAND,
        SKILL_THERAPY_PERCENT,
        THERAPY_COEFFICIENT,
        BE_THERAPY_COEFFICIENT,
        SKILL_ADAPTIVE_DAMAGE,
        SKILL_ADAPTIVE_DAMAGE_RAND,
        SOLAR_AND_LUNAR_ATTACK_POWER_BASE,
        SOLAR_AND_LUNAR_HIT_VALUE,
        SOLAR_AND_LUNAR_CRITICAL_STRIKE,
        SOLAR_AND_LUNAR_CRITICAL_DAMAGE_POWER_BASE,
        SOLAR_AND_LUNAR_OVERCOME_BASE,
        NONE_WEAPON_ATTACK_SPEED_BASE,
        MELEE_WEAPON_ATTACK_SPEED_BASE,
        MELEE_WEAPON_ATTACK_SPEED_PERCENT_ADD,
        MELEE_WEAPON_ATTACK_SPEED_ADDITIONAL,
        RANGE_WEAPON_ATTACK_SPEED_BASE,
        RANGE_WEAPON_ATTACK_SPEED_PERCENT_ADD,
        RANGE_WEAPON_ATTACK_SPEED_ADDITIONAL,
        IGNORE_OTHER_ALARM_RANGE,
        SELF_ALARM_RANGE_REVISE,
        OTHER_ALARM_RANGE_REVISE,
        DASH,
        DASH_TO_POINT,
        DASH_FORWARD,
        DASH_BACKWARD,
        DASH_LEFT,
        DASH_RIGHT,
        DASH_TO_DST_BACK,
        SKILL_MOVE,
        CATCH_PLAYER,
        HALT,
        FREEZE,
        ENTRAP,
        LOCKED_FACE,
        FEAR,
        REVIVE,
        JUMP,
        SIT,
        DISARM,
        DISABLE_MOVE_CTRL,
        DISABLE_SELECT_TARGET,
        CALL_KNOCKED_BACK_EXHALE,
        CALL_KNOCKED_BACK_CONVERGENCE,
        CALL_KNOCKED_BACK_RANDOM,
        CALL_KNOCKED_BACK_SRC_FACE,
        CALL_KNOCKED_OFF_PARABOLA,
        CALL_KNOCKED_DOWN,
        CALL_ADD_KNOCKED_DOWN_FRAME,
        CALL_REPULSED,
        SURPLUS_VALUE_BASE,
        SURPLUS_VALUE_ADD_PERCENT,
        CALL_SURPLUS_PHYSICS_DAMAGE,
        CALL_SURPLUS_SOLAR_DAMAGE,
        CALL_SURPLUS_NEUTRAL_DAMAGE,
        CALL_SURPLUS_LUNAR_DAMAGE,
        CALL_SURPLUS_POISON_DAMAGE,
        CALL_THERAPY,
        CALL_RECHARGEABLE_ABSORB_SHIELD_BY_THERAPY,
        CALL_PHYSICS_DAMAGE,
        CALL_SOLAR_DAMAGE,
        CALL_NEUTRAL_DAMAGE,
        CALL_LUNAR_DAMAGE,
        CALL_POISON_DAMAGE,
        CALL_ADAPTIVE_DAMAGE,
        CAST_SKILL_TARGET_SRC,
        CAST_SKILL_TARGET_DST,
        SKILL_EVENT_HANDLER,
        ACCUMULATE,
        CALL_BUFF,
        CALL_BUFF_STACK_2,
        CALL_BUFF_STACK_3,
        CALL_BUFF_STACK_4,
        CALL_BUFF_STACK_5,
        CALL_BUFF_STACK_6,
        CALL_BUFF_STACK_7,
        CALL_BUFF_STACK_8,
        CALL_BUFF_ACCORDING_TO_ATTRACTION,
        DETACH_SINGLE_BUFF,
        DETACH_MULTI_GROUP_BUFF,
        DEL_SINGLE_BUFF_BY_FUNCTIONTYPE,
        DEL_MULTI_GROUP_BUFF_BY_FUNCTIONTYPE,
        DEL_SINGLE_BUFF_BY_ID_AND_LEVEL,
        DEL_SINGLE_GROUP_BUFF_BY_ID_AND_LEVEL,
        DEL_MULTI_GROUP_BUFF_BY_ID,
        MODIFY_THREAT,
        MODIFY_ALL_THREATEN,
        MODIFY_THREAT_PERCENT,
        MODIFY_ALL_THREATEN_PERCENT,
        SPOOF,
        SILENCE,
        SILENCE_ALL,
        IMMUNITY,
        ADD_WEAK,
        SHIFT_WEAK,
        ADD_WEAK_SHIELD,
        MODIFY_SKILL_CAST_FLAG,
        EXECUTE_SCRIPT,
        EXECUTE_SCRIPT_WITH_PARAM,
        PRINK_REPRESENT_ID,
        PRINK_MODEL,
        PRINK_FORCE,
        RIDE_HORSE,
        DO_ACTION,
        ADD_EXP_PERCENT,
        ADD_EXP_PERCENT_BY_QUEST,
        ADD_EXP_PERCENT_BY_SCRIPT,
        ADD_REPUTATION_PERCENT,
        ADD_QUEST_REPUTATION_PERCENT,
        ADD_JUSTICE_PERCENT,
        ADD_PRESTIGE_PERCENT,
        ADD_TRAIN_PERCENT,
        DISABLE_ACQUIRE_TITLE_POINT,
        ADD_CONTRIBUTION_PERCENT_FOR_ALL,
        ADD_PRESTIGE_PERCENT_FOR_ALL,
        EMPLOYER_CAST_SKILL_TARGET_SELF,
        EMPLOYER_CAST_SKILL_TARGET_DEST,
        CLEAR_COOL_DOWN,
        RESET_COOL_DOWN,
        MODIFY_COOL_DOWN,
        ADD_COOL_DOWN_INTERVAL,
        TRANSFER_DAMAGE_TO_MANA_PERCENT,
        MODIFY_COST_MANA_PERCENT,
        MODIFY_COST_RAGE_PERCENT,
        MODIFY_COST_ENERGY_PERCENT,
        MODIFY_COST_SUN_ENERGY_PERCENT,
        DIRECT_CAST_MASK,
        PULL,
        PULL_Z,
        TURN_TO_SRC,
        CONSUME_BUFF,
        TOP_THREAT,
        ZERO_THREAT,
        FORMATION_EFFECT,
        ADD_DAMAGE_BY_DST_MOVE_STATE,
        TRANSFER_DAMAGE,
        COPY_DAMAGE,
        TRANSFER_THERAPY,
        TRANSFER_THREAT,
        ADD_FELLOWSHIP_ATTRACTION,
        CALL_HELP,
        CALL_REQUEST,
        CATCH_CHARACTER,
        TRANSMISSION,
        VITALITY_TO_PARRY_VALUE_COF,
        VITALITY_TO_MAX_LIFE_COF,
        VITALITY_TO_PHYSICS_ATTACK_POWER_COF,
        VITALITY_TO_SOLAR_ATTACK_POWER_COF,
        VITALITY_TO_LUNAR_ATTACK_POWER_COF,
        VITALITY_TO_NEUTRAL_ATTACK_POWER_COF,
        VITALITY_TO_POISON_ATTACK_POWER_COF,
        VITALITY_TO_THERAPY_POWER_COF,
        VITALITY_TO_PHYSICS_SHIELD_COF,
        VITALITY_TO_DODGE_COF,
        VITALITY_TO_PARRY_COF,
        VITALITY_TO_MAGIC_SHIELD_COF,
        VITALITY_TO_MAX_MANA_COF,
        VITALITY_TO_PHYSICS_OVER_COME_COF,
        VITALITY_TO_MAGIC_OVER_COME_COF,
        VITALITY_TO_MAGIC_CRITICAL_STRIKE_COF,
        SPIRIT_TO_MAX_MANA_COF,
        SPIRIT_TO_MANA_REPLENISH_COF,
        SPIRIT_TO_THERAPY_POWER_COF,
        SPIRIT_TO_PHYSICS_ATTACK_POWER_COF,
        SPIRIT_TO_SOLAR_ATTACK_POWER_COF,
        SPIRIT_TO_LUNAR_ATTACK_POWER_COF,
        SPIRIT_TO_NEUTRAL_ATTACK_POWER_COF,
        SPIRIT_TO_POISON_ATTACK_POWER_COF,
        SPIRIT_TO_PHYSICS_CRITICAL_STRIKE_COF,
        SPIRIT_TO_SOLAR_CRITICAL_STRIKE_COF,
        SPIRIT_TO_LUNAR_CRITICAL_STRIKE_COF,
        SPIRIT_TO_NEUTRAL_CRITICAL_STRIKE_COF,
        SPIRIT_TO_POISON_CRITICAL_STRIKE_COF,
        SPIRIT_TO_PHYSICS_CRITICAL_DAMAGE_POWER_COF,
        SPIRIT_TO_SOLAR_CRITICAL_DAMAGE_POWER_COF,
        SPIRIT_TO_LUNAR_CRITICAL_DAMAGE_POWER_COF,
        SPIRIT_TO_NEUTRAL_CRITICAL_DAMAGE_POWER_COF,
        SPIRIT_TO_POISON_CRITICAL_DAMAGE_POWER_COF,
        LUNAR_ATTACK_POWER_TO_THERAPY_POWER_COF,
        THERAPY_POWER_TO_MAGIC_ATTACK_POWER_COF,
        THERAPY_POWER_TO_PHYSICS_ATTACK_POWER_COF,
        NEUTRAL_ATTACK_POWER_TO_THERAPY_POWER_COF,
        SPIRIT_TO_SOLAR_MAGIC_SHIELD_COF,
        SPIRIT_TO_LUNAR_MAGIC_SHIELD_COF,
        SPIRIT_TO_NEUTRAL_MAGIC_SHIELD_COF,
        SPIRIT_TO_POISON_MAGIC_SHIELD_COF,
        SPIRIT_TO_POISON_OVERCOME_COF,
        SPUNK_TO_MANA_REPLENISH_COF,
        SPUNK_TO_PHYSICS_ATTACK_POWER_COF,
        SPUNK_TO_SOLAR_ATTACK_POWER_COF,
        SPUNK_TO_LUNAR_ATTACK_POWER_COF,
        SPUNK_TO_NEUTRAL_ATTACK_POWER_COF,
        SPUNK_TO_POISON_ATTACK_POWER_COF,
        SPUNK_TO_SOLAR_AND_LUNAR_ATTACK_POWER_COF,
        SPUNK_TO_SOLAR_CRITICAL_STRIKE_COF,
        SPUNK_TO_SOLAR_CRITICAL_DAMAGE_POWER_COF,
        SPUNK_TO_SOLAR_AND_LUNAR_CRITICAL_STRIKE_COF,
        SPUNK_TO_SOLAR_AND_LUNAR_CRITICAL_DAMAGE_POWER_COF,
        SPUNK_TO_PHYSICS_CRITICAL_STRIKE_COF,
        SPUNK_TO_PHYSICS_CRITICAL_DAMAGE_POWER_COF,
        SPUNK_TO_NEUTRAL_OVERCOME_COF,
        SPUNK_TO_NEUTRAL_CRITICAL_STRIKE_COF,
        STRENGTH_TO_PHYSICS_CRITICAL_STRIKE_COF,
        STRENGTH_TO_PHYSICS_ATTACK_POWER_COF,
        STRENGTH_TO_PHYSICS_CRITICAL_DAMAGE_POWER_COF,
        STRENGTH_TO_SOLAR_ATTACK_POWER_COF,
        STRENGTH_TO_LUNAR_ATTACK_POWER_COF,
        STRENGTH_TO_NEUTRAL_ATTACK_POWER_COF,
        STRENGTH_TO_POISON_ATTACK_POWER_COF,
        STRENGTH_TO_PHYSICS_OVERCOME_COF,
        AGILITY_TO_PHYSICS_CRITICAL_STRIKE_COF,
        AGILITY_TO_PHYSICS_CRITICAL_DAMAGE_POWER_COF,
        AGILITY_TO_POISON_CRITICAL_STRIKE_COF,
        AGILITY_TO_PHYSICS_ATTACK_POWER_COF,
        AGILITY_TO_SOLAR_AND_LUNAR_ATTACK_POWER_COF,
        AGILITY_TO_PHYSICS_OVERCOME_COF,
        AGILITY_TO_DODGE_COF,
        AGILITY_TO_PARRY_COF,
        AGILITY_TO_PARRY_VALUE_COF,
        STEALTH,
        COUNTER_STEALTH,
        COUNTER_STEALTH_RANGE,
        TRANSPARENCY_VALUE,
        AUTO_SELECT_TARGET,
        CALL_MENTOR_BUFF,
        MENTOR_FORMATION_EFFECT,
        ADJUST_PROFESSION_LEVEL,
        SET_USE_BIG_SWORD_FLAG,
        CHANGE_CARRIER_STATE,
        SET_INCOME_LIMIT_FLAG,
        SET_SALE_LIMIT_FLAG,
        SET_DISABLE_SKILL_CAST_ON_ENTRAP_FLAG,
        SET_EQUIPMENT_RECIPE,
        SET_TALENT_RECIPE,
        TRANSFER_LIFE,
        TRANSFER_MANA,
        COPY_EFFECTIVE_THERAPY,
        AUTO_SELECT_TARGET_BY_LIFELESS,
        CAST_SUB_SKILL,
        CAST_SKILL,
        SET_DISABLE_TITLEPOINT_PRODUCE_FLAG,
        ADD_SPRINT_POWER_MAX,
        ADD_SPRINT_POWER_COST,
        ADD_SPRINT_POWER_REVIVE,
        ADD_SPRINT_POWER_COST_ON_WALL,
        ADD_SPRINT_POWER_COST_RUN_ON_WALL_EXTRA,
        ADD_SPRINT_POWER_COST_STAND_ON_WALL,
        ADD_FLY_SPRINT_POWER_COST,
        DISABLE_SPRINT_FLAG,
        DISABLE_SPRINT_POWER_COST,
        SPRINT_POWER_ADD_PERCENT,
        ADD_HORSE_SPRINT_POWER_MAX,
        ADD_HORSE_SPRINT_POWER_COST,
        ADD_HORSE_SPRINT_POWER_REVIVE,
        ENABLE_DOUBLE_RIDE,
        HORSE_CAN_SWIM,
        CHANGE_SKILL_ICON,
        CHANGE_NO_LIMIT_SKILL_ICON,
        ADD_SCALE,
        DISABLE_REVIVE,
        DISABLE_TEAMWORK,
        IMMUNITY_BEAT_BREAK,
        ADD_TRANSPARENCY_VALUE,
        ALL_DAMAGE_ADD_PERCENT,
        ALL_SHIELD_IGNORE_PERCENT,
        NOT_IGNORE,
        CATCH_DASH,
        CHANGE_POSE_STATE,
        RECOVERY_SHIELD,
        MIRROR_SHIELD,
        ADD_GLOBAL_ABSORB_SHIELD_BY_PARRY,
        ADD_GLOBAL_ABSORB_SHIELD_BY_PARRY_VALUE,
        ADD_SWIM_WATER_LINE_COF,
        ADD_ENFORCE_WATER_FLOAT_COF,
        PAUSE_CD_TIMER,
        ACCELERATE_CD_TIMER,
        EQUAL_DIVISION_TEAM_DAMAGE,
        EQUAL_DIVISION_TEAM_THREAPY,
        IMMUNITY_CAST_ID,
        IGNORE_GRAVITY,
        FIXED_DAMAGE_REDUCE_VALUE,
        REDUCE_DAMAGE_WHEN_LIFE_CHANGED,
        REDUCE_DAMAGE_WHEN_LIFE_LOW,
        STEAL_BUFF,
        MARKED_BUSTUP_BY_GUARD,
        DRIFT_FLAG,
        SET_SELECTABLE_TYPE,
        SET_HIT_REDUCE_RATE,
        BUFF_FUNCTION_DECAY,
        WATER_FLOAT,
        MODIFY_REFLECTION_PERCENT,
        SET_PLAYER_UNCONTROLLABLE,
        MAX_SKILL_REDIUS_ADD,
        SET_RELATION_ALL_ENEMY_EXCEPT_TEAM,
        DAMAGE_TO_MANA_FOR_DST,
        DAMAGE_TO_SPRINT_POWER_FOR_DST,
        REDUCE_AOE_DAMAGE_PERCENT,
        GLOBAL_DAMGAGE_FACTOR,
        DECAY_VELOCITY,
        HOLD_HORSE,
        PRE_DIE_EXECUTE_SCRIPT,
        SHIELD_TRANSFER,
        KNOCK_DOWN_TIME_RATE,
        BLOCK_LONG_RANGE,
        ISOLATED,
        SPECIAL_SKILL_MOVE,
        MODIFY_DIVING_COUNT,
        STEAL_SOLAR_LUNAR_NEUTRAL_POSION_BUFF,
        DST_NPC_DAMAGE_COEFFICIENT,
        DAMAGE_TO_THERAPY,
        STOP,
        STOP_XYZ,
        COUNT,
    };
    enum class BUFF_COMPARE_FLAG {
        EQUAL,
        GREATER,
        GREATER_EQUAL,
        COUNT,
    };
};

// 定义 lua 中的表.

} // namespace interface

#endif // MAIN_INTERFACE_FUNC_H_