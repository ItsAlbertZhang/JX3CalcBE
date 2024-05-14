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
    atCastSkillTargetDst,
    atDamageToLifeForSelf,
    atExecuteScript,
    atHalt,
    atImmuneSkillMove,
    atImmunity,
    atKnockedDownRate,
    atKnockedOffRate,
    atLunarAttackPowerPercent,
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
    atSolarAttackPowerPercent,
    atSolarCriticalDamagePowerBaseKiloNumRate,
    atSolarCriticalStrikeBaseRate,
    atSolarDamageCoefficient,
    atStealth,
    COUNT,
};

} // namespace frame
} // namespace jx3calc
