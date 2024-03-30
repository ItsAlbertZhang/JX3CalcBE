#pragma once

#include "frame/ref/ref.h"

namespace jx3calc {
namespace frame {

enum class ref::Attrib {
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

} // namespace frame
} // namespace jx3calc
