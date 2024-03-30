#pragma once

#include "frame/ref/ref.h"

namespace jx3calc {
namespace frame {

template <>
class Ref<ref::Attrib> {
public:
    enum class Type {
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

    inline static const char *list[]{
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

    inline static const std::unordered_map<std::string, Type> map{
        {"atActiveThreatCoefficient",                 Type::atActiveThreatCoefficient                },
        {"atAddTransparencyValue",                    Type::atAddTransparencyValue                   },
        {"atAllDamageAddPercent",                     Type::atAllDamageAddPercent                    },
        {"atAllMagicDamageAddPercent",                Type::atAllMagicDamageAddPercent               },
        {"atAllShieldIgnorePercent",                  Type::atAllShieldIgnorePercent                 },
        {"atBeImmunisedStealthEnable",                Type::atBeImmunisedStealthEnable               },
        {"atBeTherapyCoefficient",                    Type::atBeTherapyCoefficient                   },
        {"atCallBuff",                                Type::atCallBuff                               },
        {"atCallLunarDamage",                         Type::atCallLunarDamage                        },
        {"atCallSolarDamage",                         Type::atCallSolarDamage                        },
        {"atExecuteScript",                           Type::atExecuteScript                          },
        {"atHalt",                                    Type::atHalt                                   },
        {"atImmuneSkillMove",                         Type::atImmuneSkillMove                        },
        {"atImmunity",                                Type::atImmunity                               },
        {"atKnockedDownRate",                         Type::atKnockedDownRate                        },
        {"atKnockedOffRate",                          Type::atKnockedOffRate                         },
        {"atLunarCriticalDamagePowerBaseKiloNumRate", Type::atLunarCriticalDamagePowerBaseKiloNumRate},
        {"atLunarCriticalStrikeBaseRate",             Type::atLunarCriticalStrikeBaseRate            },
        {"atLunarDamageCoefficient",                  Type::atLunarDamageCoefficient                 },
        {"atMagicCriticalDamagePowerBaseKiloNumRate", Type::atMagicCriticalDamagePowerBaseKiloNumRate},
        {"atMagicOvercome",                           Type::atMagicOvercome                          },
        {"atMoveSpeedPercent",                        Type::atMoveSpeedPercent                       },
        {"atNoLimitChangeSkillIcon",                  Type::atNoLimitChangeSkillIcon                 },
        {"atSetSelectableType",                       Type::atSetSelectableType                      },
        {"atSetTalentRecipe",                         Type::atSetTalentRecipe                        },
        {"atSkillEventHandler",                       Type::atSkillEventHandler                      },
        {"atSolarCriticalDamagePowerBaseKiloNumRate", Type::atSolarCriticalDamagePowerBaseKiloNumRate},
        {"atSolarCriticalStrikeBaseRate",             Type::atSolarCriticalStrikeBaseRate            },
        {"atSolarDamageCoefficient",                  Type::atSolarDamageCoefficient                 },
        {"atStealth",                                 Type::atStealth                                },
    };
};

} // namespace frame
} // namespace jx3calc
