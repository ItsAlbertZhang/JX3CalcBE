#ifndef FRAME_REF_TAB_ATTRIBUTE_H_
#define FRAME_REF_TAB_ATTRIBUTE_H_

#include <string>
#include <unordered_map>

namespace ns_frame {
namespace ref {

enum class enumTabAttribute {
    atActiveThreatCoefficient,
    atAddTransparencyValue,
    atAllDamageAddPercent,
    atAllMagicDamageAddPercent,
    atAllShieldIgnorePercent,
    atBeImmunisedStealthEnable,
    atBeTherapyCoefficient,
    atCallBuff,
    atCallLunarDamage,
    atCallSolarDamage,
    atExecuteScript,
    atHalt,
    atImmuneSkillMove,
    atImmunity,
    atKnockedDownRate,
    atKnockedOffRate,
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
    {"atActiveThreatCoefficient",                 enumTabAttribute::atActiveThreatCoefficient                },
    {"atAddTransparencyValue",                    enumTabAttribute::atAddTransparencyValue                   },
    {"atAllDamageAddPercent",                     enumTabAttribute::atAllDamageAddPercent                    },
    {"atAllMagicDamageAddPercent",                enumTabAttribute::atAllMagicDamageAddPercent               },
    {"atAllShieldIgnorePercent",                  enumTabAttribute::atAllShieldIgnorePercent                 },
    {"atBeImmunisedStealthEnable",                enumTabAttribute::atBeImmunisedStealthEnable               },
    {"atBeTherapyCoefficient",                    enumTabAttribute::atBeTherapyCoefficient                   },
    {"atCallBuff",                                enumTabAttribute::atCallBuff                               },
    {"atCallLunarDamage",                         enumTabAttribute::atCallLunarDamage                        },
    {"atCallSolarDamage",                         enumTabAttribute::atCallSolarDamage                        },
    {"atExecuteScript",                           enumTabAttribute::atExecuteScript                          },
    {"atHalt",                                    enumTabAttribute::atHalt                                   },
    {"atImmuneSkillMove",                         enumTabAttribute::atImmuneSkillMove                        },
    {"atImmunity",                                enumTabAttribute::atImmunity                               },
    {"atKnockedDownRate",                         enumTabAttribute::atKnockedDownRate                        },
    {"atKnockedOffRate",                          enumTabAttribute::atKnockedOffRate                         },
    {"atLunarCriticalStrikeBaseRate",             enumTabAttribute::atLunarCriticalStrikeBaseRate            },
    {"atLunarDamageCoefficient",                  enumTabAttribute::atLunarDamageCoefficient                 },
    {"atMagicCriticalDamagePowerBaseKiloNumRate", enumTabAttribute::atMagicCriticalDamagePowerBaseKiloNumRate},
    {"atMoveSpeedPercent",                        enumTabAttribute::atMoveSpeedPercent                       },
    {"atNoLimitChangeSkillIcon",                  enumTabAttribute::atNoLimitChangeSkillIcon                 },
    {"atSetSelectableType",                       enumTabAttribute::atSetSelectableType                      },
    {"atSetTalentRecipe",                         enumTabAttribute::atSetTalentRecipe                        },
    {"atSkillEventHandler",                       enumTabAttribute::atSkillEventHandler                      },
    {"atSolarCriticalStrikeBaseRate",             enumTabAttribute::atSolarCriticalStrikeBaseRate            },
    {"atSolarDamageCoefficient",                  enumTabAttribute::atSolarDamageCoefficient                 },
    {"atStealth",                                 enumTabAttribute::atStealth                                },
};

} // namespace ref
} // namespace ns_frame

#endif // FRAME_REF_TAB_ATTRIBUTE_H_
