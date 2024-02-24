#include "frame/character/property/attribute.h"
#include "frame/globalparam.h"

using namespace ns_frame;

// 主属性
int ChAttr::getVitality() const {
    return atBasePotentialAdd + atVitalityBase +
           atVitalityBase * atVitalityBasePercentAdd / 1024;
}
int ChAttr::getStrength() const {
    return atBasePotentialAdd + atStrengthBase +
           atStrengthBase * atStrengthBasePercentAdd / 1024;
}
int ChAttr::getAgility() const {
    return atBasePotentialAdd + atAgilityBase +
           atAgilityBase * atAgilityBasePercentAdd / 1024;
}
int ChAttr::getSpirit() const {
    return atBasePotentialAdd + atSpiritBase +
           atSpiritBase * atSpiritBasePercentAdd / 1024;
}
int ChAttr::getSpunk() const {
    return atBasePotentialAdd + atSpunkBase +
           atSpunkBase * atSpunkBasePercentAdd / 1024;
}

// 攻击力
int ChAttr::getPhysicsAttackPower() const {
    return atPhysicsAttackPowerBase +
           atPhysicsAttackPowerBase * atPhysicsAttackPowerPercent / 1024;
}
int ChAttr::getSolarAttackPower() const {
    return (atSolarAttackPowerBase + atMagicAttackPowerBase) +
           (atSolarAttackPowerBase + atMagicAttackPowerBase) *
               (atSolarAttackPowerPercent + atMagicAttackPowerPercent) / 1024 +
           atSpunkToSolarAndLunarAttackPowerCof * getSpunk() / 1024;
}
int ChAttr::getLunarAttackPower() const {
    return (atLunarAttackPowerBase + atMagicAttackPowerBase) +
           (atLunarAttackPowerBase + atMagicAttackPowerBase) *
               (atLunarAttackPowerPercent + atMagicAttackPowerPercent) / 1024 +
           atSpunkToSolarAndLunarAttackPowerCof * getSpunk() / 1024;
}
int ChAttr::getNeutralAttackPower() const {
    return (atNeutralAttackPowerBase + atMagicAttackPowerBase) +
           (atNeutralAttackPowerBase + atMagicAttackPowerBase) *
               (atNeutralAttackPowerPercent + atMagicAttackPowerPercent) / 1024;
}
int ChAttr::getPoisonAttackPower() const {
    return (atPoisonAttackPowerBase + atMagicAttackPowerBase) +
           (atPoisonAttackPowerBase + atMagicAttackPowerBase) *
               (atPoisonAttackPowerPercent + atMagicAttackPowerPercent) / 1024;
}

// 会心等级
int ChAttr::getPhysicsCriticalStrikeValue() const {
    return atPhysicsCriticalStrike + atAllTypeCriticalStrike + atPhysicsCriticalStrikeAdd;
}
int ChAttr::getSolarCriticalStrikeValue() const {
    return atSolarCriticalStrike + atAllTypeCriticalStrike + atMagicCriticalStrike + atSolarCriticalStrikeAdd +
           atSpunkToSolarAndLunarCriticalStrikeCof * getSpunk() / 1024;
}
int ChAttr::getLunarCriticalStrikeValue() const {
    return atLunarCriticalStrike + atAllTypeCriticalStrike + atMagicCriticalStrike + atLunarCriticalStrikeAdd +
           atSpunkToSolarAndLunarCriticalStrikeCof * getSpunk() / 1024;
}
int ChAttr::getNeutralCriticalStrikeValue() const {
    return atNeutralCriticalStrike + atAllTypeCriticalStrike + atMagicCriticalStrike + atNeutralCriticalStrikeAdd;
}
int ChAttr::getPoisonCriticalStrikeValue() const {
    return atPoisonCriticalStrike + atAllTypeCriticalStrike + atMagicCriticalStrike + atPoisonCriticalStrikeAdd;
}

// 会心
int ChAttr::getPhysicsCriticalStrike() const {
    int res;
    res = getPhysicsCriticalStrikeValue();
    res = static_cast<int>(res * 10000 / (GlobalParam::get().fCriticalStrikeParam * GlobalParam::levelCof(atLevel))) +
          atPhysicsCriticalStrikeBaseRate;
    return res;
}
int ChAttr::getSolarCriticalStrike() const {
    int res;
    res = getSolarCriticalStrikeValue();
    res = static_cast<int>(res * 10000 / (GlobalParam::get().fCriticalStrikeParam * GlobalParam::levelCof(atLevel))) +
          atSolarCriticalStrikeBaseRate;
    return res;
}
int ChAttr::getLunarCriticalStrike() const {
    int res;
    res = getLunarCriticalStrikeValue();
    res = static_cast<int>(res * 10000 / (GlobalParam::get().fCriticalStrikeParam * GlobalParam::levelCof(atLevel))) +
          atLunarCriticalStrikeBaseRate;
    return res;
}
int ChAttr::getNeutralCriticalStrike() const {
    int res;
    res = getNeutralCriticalStrikeValue();
    res = static_cast<int>(res * 10000 / (GlobalParam::get().fCriticalStrikeParam * GlobalParam::levelCof(atLevel))) +
          atNeutralCriticalStrikeBaseRate;
    return res;
}
int ChAttr::getPoisonCriticalStrike() const {
    int res;
    res = getPoisonCriticalStrikeValue();
    res = static_cast<int>(res * 10000 / (GlobalParam::get().fCriticalStrikeParam * GlobalParam::levelCof(atLevel))) +
          atPoisonCriticalStrikeBaseRate;
    return res;
}

// 会心效果
int ChAttr::getPhysicsCriticalDamagePower() const {
    int res;
    res = atAllTypeCriticalDamagePowerBase + atPhysicsCriticalDamagePowerBase;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fCriticalStrikePowerParam * GlobalParam::levelCof(atLevel)));
    res = res + atPhysicsCriticalDamagePowerBaseKiloNumRate;
    return res > 1280 ? 1280 : res;
}
int ChAttr::getSolarCriticalDamagePower() const {
    int res;
    res = atSolarCriticalDamagePowerBase + atAllTypeCriticalDamagePowerBase + atMagicCriticalDamagePowerBase;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fCriticalStrikePowerParam * GlobalParam::levelCof(atLevel)));
    res = res + atSolarCriticalDamagePowerBaseKiloNumRate + atMagicCriticalDamagePowerBaseKiloNumRate;
    return res > 1280 ? 1280 : res;
}
int ChAttr::getLunarCriticalDamagePower() const {
    int res;
    res = atLunarCriticalDamagePowerBase + atAllTypeCriticalDamagePowerBase + atMagicCriticalDamagePowerBase;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fCriticalStrikePowerParam * GlobalParam::levelCof(atLevel)));
    res = res + atLunarCriticalDamagePowerBaseKiloNumRate + atMagicCriticalDamagePowerBaseKiloNumRate;
    return res > 1280 ? 1280 : res;
}
int ChAttr::getNeutralCriticalDamagePower() const {
    int res;
    res = atNeutralCriticalDamagePowerBase + atAllTypeCriticalDamagePowerBase + atMagicCriticalDamagePowerBase;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fCriticalStrikePowerParam * GlobalParam::levelCof(atLevel)));
    res = res + atNeutralCriticalDamagePowerBaseKiloNumRate + atMagicCriticalDamagePowerBaseKiloNumRate;
    return res > 1280 ? 1280 : res;
}
int ChAttr::getPoisonCriticalDamagePower() const {
    int res;
    res = atPoisonCriticalDamagePowerBase + atAllTypeCriticalDamagePowerBase + atMagicCriticalDamagePowerBase;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fCriticalStrikePowerParam * GlobalParam::levelCof(atLevel)));
    res = res + atPoisonCriticalDamagePowerBaseKiloNumRate + atMagicCriticalDamagePowerBaseKiloNumRate;
    return res > 1280 ? 1280 : res;
}

// 破防
int ChAttr::getPhysicsOvercome() const {
    int res;
    res = atPhysicsOvercomeBase;
    res = res + res * atPhysicsOvercomePercent / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fOvercomeParam * GlobalParam::levelCof(atLevel)));
    return res;
}
int ChAttr::getSolarOvercome() const {
    int res;
    res = atSolarOvercomeBase + atMagicOvercome;
    res = res + res * atSolarOvercomePercent / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fOvercomeParam * GlobalParam::levelCof(atLevel)));
    return res;
}
int ChAttr::getLunarOvercome() const {
    int res;
    res = atLunarOvercomeBase + atMagicOvercome;
    res = res + res * atLunarOvercomePercent / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fOvercomeParam * GlobalParam::levelCof(atLevel)));
    return res;
}
int ChAttr::getNeutralOvercome() const {
    int res;
    res = atNeutralOvercomeBase + atMagicOvercome;
    res = res + res * atNeutralOvercomePercent / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fOvercomeParam * GlobalParam::levelCof(atLevel)));
    return res;
}
int ChAttr::getPoisonOvercome() const {
    int res;
    res = atPoisonOvercomeBase + atMagicOvercome;
    res = res + res * atPoisonOvercomePercent / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fOvercomeParam * GlobalParam::levelCof(atLevel)));
    return res;
}

// 防御
int ChAttr::getPhysicsShield(int srcShieldIgnorePercent) const {
    int res = (atPhysicsShieldBase +
               atPhysicsShieldBase * atPhysicsShieldPercent / 1024 +
               atPhysicsShieldAdditional) *
              (1024 - srcShieldIgnorePercent) / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fPhysicsShieldParam * GlobalParam::levelCof(atLevel) + res));
    return res > 768 ? 768 : res;
}
int ChAttr::getSolarShield(int srcShieldIgnorePercent) const {
    int res = ((atSolarMagicShieldBase + atMagicShield) +
               (atSolarMagicShieldBase + atMagicShield) * atSolarMagicShieldPercent / 1024) *
              (1024 - srcShieldIgnorePercent) / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fMagicShieldParam * GlobalParam::levelCof(atLevel) + res));
    return res > 768 ? 768 : res;
}
int ChAttr::getLunarShield(int srcShieldIgnorePercent) const {
    int res = ((atLunarMagicShieldBase + atMagicShield) +
               (atLunarMagicShieldBase + atMagicShield) * atLunarMagicShieldPercent / 1024) *
              (1024 - srcShieldIgnorePercent) / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fMagicShieldParam * GlobalParam::levelCof(atLevel) + res));
    return res > 768 ? 768 : res;
}
int ChAttr::getNeutralShield(int srcShieldIgnorePercent) const {
    int res = ((atNeutralMagicShieldBase + atMagicShield) +
               (atNeutralMagicShieldBase + atMagicShield) * atNeutralMagicShieldPercent / 1024) *
              (1024 - srcShieldIgnorePercent) / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fMagicShieldParam * GlobalParam::levelCof(atLevel) + res));
    return res > 768 ? 768 : res;
}
int ChAttr::getPoisonShield(int srcShieldIgnorePercent) const {
    int res = ((atPoisonMagicShieldBase + atMagicShield) +
               (atPoisonMagicShieldBase + atMagicShield) * atPoisonMagicShieldPercent / 1024) *
              (1024 - srcShieldIgnorePercent) / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fMagicShieldParam * GlobalParam::levelCof(atLevel) + res));
    return res > 768 ? 768 : res;
}

// 无双
int ChAttr::getStrain() const {
    int res;
    res = atStrainBase;
    res = res + res * atStrainPercent / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fInsightParam * GlobalParam::levelCof(atLevel)));
    res = res + atStrainRate;
    return res;
}
// 急速
int ChAttr::getHaste() const {
    int res;
    res = atHasteBase;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fHasteRate * GlobalParam::levelCof(atLevel)));
    res = res + atHasteBasePercentAdd;
    res = res > 256 ? 256 : res;
    return res + atUnlimitHasteBasePercentAdd;
}
// 破招
int ChAttr::getSurplus() const {
    return static_cast<int>(atSurplusValueBase * GlobalParam::get().fSurplusParam);
}

// 伤害加成
int ChAttr::getPhysicsDamageAddPercent() const {
    return atAllDamageAddPercent;
}
int ChAttr::getSolarDamageAddPercent() const {
    return atAllDamageAddPercent + atAllMagicDamageAddPercent;
}
int ChAttr::getLunarDamageAddPercent() const {
    return atAllDamageAddPercent + atAllMagicDamageAddPercent;
}
int ChAttr::getNeutralDamageAddPercent() const {
    return atAllDamageAddPercent + atAllMagicDamageAddPercent;
}
int ChAttr::getPoisonDamageAddPercent() const {
    return atAllDamageAddPercent + atAllMagicDamageAddPercent;
}
