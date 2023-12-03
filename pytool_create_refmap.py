# encoding: utf-8
import sys

list_to_create = {
    "LuaAttributeType": {
        "map": False,
        "list": [
            "CAST_SKILL",
            "CAST_SKILL_TARGET_DST",
            "EXECUTE_SCRIPT",
            "EXECUTE_SCRIPT_WITH_PARAM",
            "SKILL_PHYSICS_DAMAGE",
            "SKILL_PHYSICS_DAMAGE_RAND",
            "SKILL_SOLAR_DAMAGE",
            "SKILL_SOLAR_DAMAGE_RAND",
            "SKILL_LUNAR_DAMAGE",
            "SKILL_LUNAR_DAMAGE_RAND",
            "SKILL_NEUTRAL_DAMAGE",
            "SKILL_NEUTRAL_DAMAGE_RAND",
            "SKILL_POISON_DAMAGE",
            "SKILL_POISON_DAMAGE_RAND",
            "CALL_PHYSICS_DAMAGE",
            "CALL_SOLAR_DAMAGE",
            "CALL_LUNAR_DAMAGE",
            "CALL_NEUTRAL_DAMAGE",
            "CALL_POISON_DAMAGE",
            "CURRENT_SUN_ENERGY",
            "CURRENT_MOON_ENERGY",
            "DASH",
            "PHYSICS_ATTACK_POWER_PERCENT",
            "SOLAR_ATTACK_POWER_PERCENT",
            "LUNAR_ATTACK_POWER_PERCENT",
            "NEUTRAL_ATTACK_POWER_PERCENT",
            "POISON_ATTACK_POWER_PERCENT",
            "DST_NPC_DAMAGE_COEFFICIENT",
            "MAGIC_SHIELD",
            "PHYSICS_SHIELD_BASE",
            "SET_TALENT_RECIPE",
            "DROP_DEFENCE",
            "ACTIVE_THREAT_COEFFICIENT",
            "BEAT_BACK_RATE",
            "SPUNK_TO_SOLAR_AND_LUNAR_ATTACK_POWER_COF",
            "SPUNK_TO_SOLAR_AND_LUNAR_CRITICAL_STRIKE_COF",
            "MAX_SUN_ENERGY",
            "MAX_MOON_ENERGY",
            "MAX_LIFE_PERCENT_ADD",
            "LUNAR_ATTACK_POWER_BASE",
            "SOLAR_ATTACK_POWER_BASE",
            "MANA_REPLENISH_PERCENT",
            "SET_ADAPTIVE_SKILL_TYPE",
            "SKILL_EVENT_HANDLER",
            "DECRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE",
            "LUNAR_CRITICAL_STRIKE_BASE_RATE",
            "SOLAR_CRITICAL_STRIKE_BASE_RATE",
            "MAGIC_CRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE",
            "CALL_BUFF",
            "GLOBAL_DAMGAGE_FACTOR",
            "CALL_SURPLUS_PHYSICS_DAMAGE",
            "CALL_SURPLUS_SOLAR_DAMAGE",
            "CALL_SURPLUS_LUNAR_DAMAGE",
            "CALL_SURPLUS_NEUTRAL_DAMAGE",
            "CALL_SURPLUS_POISON_DAMAGE",
            "DEL_MULTI_GROUP_BUFF_BY_FUNCTIONTYPE",
            "DEL_SINGLE_BUFF_BY_ID_AND_LEVEL",
            "SOLAR_CRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE",
            "LUNAR_CRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE",
        ],
    },
    "TabAttribute": {
        "map": True,
        "list": [
            "atAllDamageAddPercent",
            "atSolarDamageCoefficient",
            "atLunarDamageCoefficient",
            "atCallLunarDamage",
            "atExecuteScript",
            "atLunarCriticalStrikeBaseRate",
            "atSolarCriticalStrikeBaseRate",
            "atMagicCriticalDamagePowerBaseKiloNumRate",
            "atAllShieldIgnorePercent",
            "atAddTransparencyValue",
            "atSetSelectableType",
            "atSkillEventHandler",
            "atStealth",
            "atMoveSpeedPercent",
            "atKnockedDownRate",
            "atBeImmunisedStealthEnable",
            "atImmunity",
            "atImmuneSkillMove",
            "atActiveThreatCoefficient",
            "atHalt",
            "atNoLimitChangeSkillIcon",
            "atSetTalentRecipe",
            "atAllMagicDamageAddPercent",
        ],
    },
}

output = """\
#ifndef FRAME_STATIC_REFMAP_H_
#define FRAME_STATIC_REFMAP_H_

#include <string>
#include <unordered_map>

namespace ns_framestatic {

"""

for key, value in list_to_create.items():
    value["list"].sort()
    output_class = "enum class enum" + key + " {\n"
    output_ref = ""
    output_map = ""
    if value["map"]:
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
    for it in value["list"]:
        output_class += "    " + it + ",\n"
        output_ref += '    "' + it + '",\n'
        if value["map"]:
            output_map += '    {"' + it + '", enum' + key + "::" + it + "},\n"
    output_class += "    COUNT,\n};\n"
    output_ref += "};\n"
    if value["map"]:
        output_map += "};\n"
    output += output_class + output_ref + output_map + "\n"

output += """\
} // namespace ns_framestatic

#endif // FRAME_STATIC_REFMAP_H_
"""

if __name__ == "__main__" and len(sys.argv) == 2:
    with open(sys.argv[1], "w") as f:
        f.write(output)
else:
    print(output)
