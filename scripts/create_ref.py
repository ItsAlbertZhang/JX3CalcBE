# encoding: utf-8
"""生成带反射的枚举头文件"""


class MyClass:
    name: str
    map: bool
    vector: list[str]

    def __init__(self, name: str, map: bool, vector: list[str]) -> None:
        self.name = name
        self.map = map
        self.vector = vector


lua_attribute_type: list[str] = [
    "ACTIVE_THREAT_COEFFICIENT",
    "ADD_DAMAGE_BY_DST_MOVE_STATE",
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
    "DEL_MULTI_GROUP_BUFF_BY_ID",
    "DEL_SINGLE_BUFF_BY_ID_AND_LEVEL",
    "DO_ACTION",
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
    "MOON_POWER_VALUE",
    "NEUTRAL_ATTACK_POWER_PERCENT",
    "NEUTRAL_CRITICAL_STRIKE_BASE_RATE",
    "PHYSICS_ATTACK_POWER_PERCENT",
    "PHYSICS_CRITICAL_STRIKE_BASE_RATE",
    "PHYSICS_SHIELD_BASE",
    "POISON_ATTACK_POWER_PERCENT",
    "POISON_CRITICAL_STRIKE_BASE_RATE",
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
    "STOP",
    "SUN_POWER_VALUE",
]
tab_attribute: list[str] = [
    "atActiveThreatCoefficient",
    "atAddTransparencyValue",
    "atAllDamageAddPercent",
    "atAllMagicDamageAddPercent",
    "atAllShieldIgnorePercent",
    "atBeImmunisedStealthEnable",
    "atBeTherapyCoefficient",
    "atCallBuff",
    "atCallLunarDamage",
    "atCallSolarDamage",
    "atExecuteScript",
    "atHalt",
    "atImmuneSkillMove",
    "atImmunity",
    "atKnockedDownRate",
    "atKnockedOffRate",
    "atLunarCriticalStrikeBaseRate",
    "atLunarDamageCoefficient",
    "atMagicCriticalDamagePowerBaseKiloNumRate",
    "atMagicOvercome",
    "atMoveSpeedPercent",
    "atNoLimitChangeSkillIcon",
    "atSetSelectableType",
    "atSetTalentRecipe",
    "atSkillEventHandler",
    "atSolarCriticalStrikeBaseRate",
    "atSolarDamageCoefficient",
    "atStealth",
    "atSolarCriticalDamagePowerBaseKiloNumRate",
    "atLunarCriticalDamagePowerBaseKiloNumRate",
]


map_to_create: dict[str, MyClass] = {
    "LuaAttributeType": MyClass("lua_attribute_type", False, lua_attribute_type),
    "TabAttribute": MyClass("tab_attribute", True, tab_attribute),
}

for key, value in map_to_create.items():
    value.vector.sort()
    output = f"""\
#pragma once

"""
    if value.map:
        output += """\
#include <string>
#include <unordered_map>

"""
    output += """\
namespace jx3calc {
namespace frame {
namespace ref {

"""

    output_class = "enum class enum" + key + " {\n"
    output_ref = ""
    output_map = ""
    if value.map:
        output_ref = "inline const std::string ref" + key + "[] = {\n"
        output_map = (
            "inline const std::unordered_map<std::string, enum"
            + key
            + "> map"
            + key
            + " = {\n"
        )
    else:
        output_ref = "inline const char *ref" + key + "[] = {\n"
    for it in value.vector:
        output_class += "    " + it + ",\n"
        output_ref += '    "' + it + '",\n'
        if value.map:
            output_map += '    {"' + it + '", enum' + key + "::" + it + "},\n"
    output_class += "    COUNT,\n};\n"
    output_ref += "};\n"
    if value.map:
        output_map += "};\n"
    output += output_class + output_ref + output_map + "\n"

    output += """\
} // namespace ref
} // namespace frame
} // namespace jx3calc

"""

    with open(f"source/include/frame/ref/{value.name}.h", "w") as f:
        f.write(output)
