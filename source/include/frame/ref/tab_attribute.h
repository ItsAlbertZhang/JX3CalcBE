#pragma once

#include <string>
#include <unordered_map>

namespace jx3calc {
namespace frame {
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
    atLunarCriticalDamagePowerBaseKiloNumRate,
    atLunarCriticalStrikeBaseRate,
    atLunarDamageCoefficient,
    atMagicCriticalDamagePowerBaseKiloNumRate,
    atMagicOvercome,
    atMoveSpeedPercent,
    atNoLimitChangeSkillIcon,
    atSetSelectableType,
    atSetTalentRecipe,
    atSkillEventHandler,
    atSolarCriticalDamagePowerBaseKiloNumRate,
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
    "atLunarCriticalDamagePowerBaseKiloNumRate",
    "atLunarCriticalStrikeBaseRate",
    "atLunarDamageCoefficient",
    "atMagicCriticalDamagePowerBaseKiloNumRate",
    "atMagicOvercome",
    "atMoveSpeedPercent",
    "atNoLimitChangeSkillIcon",
    "atSetSelectableType",
    "atSetTalentRecipe",
    "atSkillEventHandler",
    "atSolarCriticalDamagePowerBaseKiloNumRate",
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
    {"atLunarCriticalDamagePowerBaseKiloNumRate", enumTabAttribute::atLunarCriticalDamagePowerBaseKiloNumRate},
    {"atLunarCriticalStrikeBaseRate",             enumTabAttribute::atLunarCriticalStrikeBaseRate            },
    {"atLunarDamageCoefficient",                  enumTabAttribute::atLunarDamageCoefficient                 },
    {"atMagicCriticalDamagePowerBaseKiloNumRate", enumTabAttribute::atMagicCriticalDamagePowerBaseKiloNumRate},
    {"atMagicOvercome",                           enumTabAttribute::atMagicOvercome                          },
    {"atMoveSpeedPercent",                        enumTabAttribute::atMoveSpeedPercent                       },
    {"atNoLimitChangeSkillIcon",                  enumTabAttribute::atNoLimitChangeSkillIcon                 },
    {"atSetSelectableType",                       enumTabAttribute::atSetSelectableType                      },
    {"atSetTalentRecipe",                         enumTabAttribute::atSetTalentRecipe                        },
    {"atSkillEventHandler",                       enumTabAttribute::atSkillEventHandler                      },
    {"atSolarCriticalDamagePowerBaseKiloNumRate", enumTabAttribute::atSolarCriticalDamagePowerBaseKiloNumRate},
    {"atSolarCriticalStrikeBaseRate",             enumTabAttribute::atSolarCriticalStrikeBaseRate            },
    {"atSolarDamageCoefficient",                  enumTabAttribute::atSolarDamageCoefficient                 },
    {"atStealth",                                 enumTabAttribute::atStealth                                },
};

} // namespace ref
} // namespace frame
} // namespace jx3calc
