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
        ],
    },
    "TabAttribute": {
        "map": True,
        "list": [
            "atAllDamageAddPercent",
            "atSolarDamageCoefficient",
            "atLunarDamageCoefficient",
            "atCallLunarDamage",
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
