#include "frame/character/character.h"

using namespace ns_frame;

void Character::attrImport(const std::string &attr) {
    // TODO: temp

    chAttr.atHasteBase = 95;   // 急速
    chAttr.atSpunkBase = 6380; // 元气

    chAttr.atPhysicsAttackPowerBase = 6;     // 外功基础攻击
    chAttr.atSolarAttackPowerBase = 19308;   // 阳性内功基础攻击
    chAttr.atLunarAttackPowerBase = 19308;   // 阴性内功基础攻击
    chAttr.atNeutralAttackPowerBase = 12721; // 混元内功基础攻击
    chAttr.atPoisonAttackPowerBase = 12721;  // 毒性内功基础攻击

    chAttr.atPhysicsCriticalStrike = 8325; // 外功会心等级
    chAttr.atSolarCriticalStrike = 15048;  // 阳性内功会心等级
    chAttr.atLunarCriticalStrike = 15048;  // 阴性内功会心等级
    chAttr.atNeutralCriticalStrike = 8715; // 混元内功会心等级
    chAttr.atPoisonCriticalStrike = 8715;  // 毒性内功会心等级

    chAttr.atPhysicsCriticalDamagePowerBase = 0; // 外功会心效果等级
    chAttr.atSolarCriticalDamagePowerBase = 188; // 阳性内功会心效果等级
    chAttr.atLunarCriticalDamagePowerBase = 188; // 阴性内功会心效果等级
    chAttr.atNeutralCriticalDamagePowerBase = 0; // 混元内功会心效果等级
    chAttr.atPoisonCriticalDamagePowerBase = 0;  // 毒性内功会心效果等级

    chAttr.atPhysicsOvercomeBase = 12; // 外功破防等级
    chAttr.atMagicOvercome = 20360;    // 内功破防等级

    chAttr.atStrainBase = 21855;          // 无双等级
    chAttr.atSurplusValueBase = 9536;     // 破招
    chAttr.atMeleeWeaponDamageBase = 240; // 武器伤害(基础, 数值上等于最小)
    chAttr.atMeleeWeaponDamageRand = 160; // 武器伤害(浮动, 数值上等于最大减去最小)
}