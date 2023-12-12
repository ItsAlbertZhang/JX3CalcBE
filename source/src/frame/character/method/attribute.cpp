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

// 会心
int ChAttr::getPhysicsCriticalStrike() const {
    return atPhysicsCriticalStrikeBaseRate +
           static_cast<int>(
               (atPhysicsCriticalStrike + atAllTypeCriticalStrike + atPhysicsCriticalStrikeAdd) *
               10000 / (GlobalParam::get().fCriticalStrikeParam * GlobalParam::levelCof(atLevel))
           );
}
int ChAttr::getSolarCriticalStrike() const {
    return atSolarCriticalStrikeBaseRate +
           static_cast<int>(
               (atSolarCriticalStrike + atAllTypeCriticalStrike + atMagicCriticalStrike + atSolarCriticalStrikeAdd +
                atSpunkToSolarAndLunarCriticalStrikeCof * getSpunk() / 1024) *
               10000 / (GlobalParam::get().fCriticalStrikeParam * GlobalParam::levelCof(atLevel))
           );
}
int ChAttr::getLunarCriticalStrike() const {
    return atLunarCriticalStrikeBaseRate +
           static_cast<int>(
               (atLunarCriticalStrike + atAllTypeCriticalStrike + atMagicCriticalStrike + atLunarCriticalStrikeAdd +
                atSpunkToSolarAndLunarCriticalStrikeCof * getSpunk() / 1024) *
               10000 / (GlobalParam::get().fCriticalStrikeParam * GlobalParam::levelCof(atLevel))
           );
}
int ChAttr::getNeutralCriticalStrike() const {
    return atNeutralCriticalStrikeBaseRate +
           static_cast<int>(
               (atNeutralCriticalStrike + atAllTypeCriticalStrike + atMagicCriticalStrike + atNeutralCriticalStrikeAdd) *
               10000 / (GlobalParam::get().fCriticalStrikeParam * GlobalParam::levelCof(atLevel))
           );
}
int ChAttr::getPoisonCriticalStrike() const {
    return atPoisonCriticalStrikeBaseRate +
           static_cast<int>(
               (atPoisonCriticalStrike + atAllTypeCriticalStrike + atMagicCriticalStrike + atPoisonCriticalStrikeAdd) *
               10000 / (GlobalParam::get().fCriticalStrikeParam * GlobalParam::levelCof(atLevel))
           );
}

// 会心效果
int ChAttr::getPhysicsCriticalDamagePower() const {
    int res = atPhysicsCriticalDamagePowerBaseKiloNumRate +
              static_cast<int>(
                  (atAllTypeCriticalDamagePowerBase + atPhysicsCriticalDamagePowerBase) *
                  1024 / (GlobalParam::get().fCriticalStrikePowerParam * GlobalParam::levelCof(atLevel))
              );
    return res > 1280 ? 1280 : res;
}
int ChAttr::getSolarCriticalDamagePower() const {
    int res = atSolarCriticalDamagePowerBaseKiloNumRate + atMagicCriticalDamagePowerBaseKiloNumRate +
              static_cast<int>(
                  (atSolarCriticalDamagePowerBase + atAllTypeCriticalDamagePowerBase + atMagicCriticalDamagePowerBase) *
                  1024 / (GlobalParam::get().fCriticalStrikePowerParam * GlobalParam::levelCof(atLevel))
              );
    return res > 1280 ? 1280 : res;
}
int ChAttr::getLunarCriticalDamagePower() const {
    int res = atLunarCriticalDamagePowerBaseKiloNumRate + atMagicCriticalDamagePowerBaseKiloNumRate +
              static_cast<int>(
                  (atLunarCriticalDamagePowerBase + atAllTypeCriticalDamagePowerBase + atMagicCriticalDamagePowerBase) *
                  1024 / (GlobalParam::get().fCriticalStrikePowerParam * GlobalParam::levelCof(atLevel))
              );
    return res > 1280 ? 1280 : res;
}
int ChAttr::getNeutralCriticalDamagePower() const {
    int res = atNeutralCriticalDamagePowerBaseKiloNumRate + atMagicCriticalDamagePowerBaseKiloNumRate +
              static_cast<int>(
                  (atNeutralCriticalDamagePowerBase + atAllTypeCriticalDamagePowerBase + atMagicCriticalDamagePowerBase) *
                  1024 / (GlobalParam::get().fCriticalStrikePowerParam * GlobalParam::levelCof(atLevel))
              );
    return res > 1280 ? 1280 : res;
}
int ChAttr::getPoisonCriticalDamagePower() const {
    int res = atPoisonCriticalDamagePowerBaseKiloNumRate + atMagicCriticalDamagePowerBaseKiloNumRate +
              static_cast<int>(
                  (atPoisonCriticalDamagePowerBase + atAllTypeCriticalDamagePowerBase + atMagicCriticalDamagePowerBase) *
                  1024 / (GlobalParam::get().fCriticalStrikePowerParam * GlobalParam::levelCof(atLevel))
              );
    return res > 1280 ? 1280 : res;
}

// 破防
int ChAttr::getPhysicsOvercome() const {
    return static_cast<int>(
        (atPhysicsOvercomeBase +
         atPhysicsOvercomeBase * atPhysicsOvercomePercent / 1024) *
        1024 / (GlobalParam::get().fOvercomeParam * GlobalParam::levelCof(atLevel))
    );
}
int ChAttr::getSolarOvercome() const {
    return static_cast<int>(
        ((atSolarOvercomeBase + atMagicOvercome) +
         (atSolarOvercomeBase + atMagicOvercome) * atSolarOvercomePercent / 1024) *
        1024 / (GlobalParam::get().fOvercomeParam * GlobalParam::levelCof(atLevel))
    );
}
int ChAttr::getLunarOvercome() const {
    return static_cast<int>(
        ((atLunarOvercomeBase + atMagicOvercome) +
         (atLunarOvercomeBase + atMagicOvercome) * atLunarOvercomePercent / 1024) *
        1024 / (GlobalParam::get().fOvercomeParam * GlobalParam::levelCof(atLevel))
    );
}
int ChAttr::getNeutralOvercome() const {
    return static_cast<int>(
        ((atNeutralOvercomeBase + atMagicOvercome) +
         (atNeutralOvercomeBase + atMagicOvercome) * atNeutralOvercomePercent / 1024) *
        1024 / (GlobalParam::get().fOvercomeParam * GlobalParam::levelCof(atLevel))
    );
}
int ChAttr::getPoisonOvercome() const {
    return static_cast<int>(
        ((atPoisonOvercomeBase + atMagicOvercome) +
         (atPoisonOvercomeBase + atMagicOvercome) * atPoisonOvercomePercent / 1024) *
        1024 / (GlobalParam::get().fOvercomeParam * GlobalParam::levelCof(atLevel))
    );
}

// 防御
int ChAttr::getPhysicsShield(int atAllShieldIgnorePercent) const {
    int res = (atPhysicsShieldBase +
               atPhysicsShieldBase * atPhysicsShieldPercent / 1024 +
               atPhysicsShieldAdditional) *
              (1024 - atAllShieldIgnorePercent) / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fPhysicsShieldParam * GlobalParam::levelCof(atLevel) + res));
    return res > 768 ? 768 : res;
}
int ChAttr::getSolarShield(int atAllShieldIgnorePercent) const {
    int res = ((atSolarMagicShieldBase + atMagicShield) +
               (atSolarMagicShieldBase + atMagicShield) * atSolarMagicShieldPercent / 1024) *
              (1024 - atAllShieldIgnorePercent) / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fMagicShieldParam * GlobalParam::levelCof(atLevel) + res));
    return res > 768 ? 768 : res;
}
int ChAttr::getLunarShield(int atAllShieldIgnorePercent) const {
    int res = ((atLunarMagicShieldBase + atMagicShield) +
               (atLunarMagicShieldBase + atMagicShield) * atLunarMagicShieldPercent / 1024) *
              (1024 - atAllShieldIgnorePercent) / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fMagicShieldParam * GlobalParam::levelCof(atLevel) + res));
    return res > 768 ? 768 : res;
}
int ChAttr::getNeutralShield(int atAllShieldIgnorePercent) const {
    int res = ((atNeutralMagicShieldBase + atMagicShield) +
               (atNeutralMagicShieldBase + atMagicShield) * atNeutralMagicShieldPercent / 1024) *
              (1024 - atAllShieldIgnorePercent) / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fMagicShieldParam * GlobalParam::levelCof(atLevel) + res));
    return res > 768 ? 768 : res;
}
int ChAttr::getPoisonShield(int atAllShieldIgnorePercent) const {
    int res = ((atPoisonMagicShieldBase + atMagicShield) +
               (atPoisonMagicShieldBase + atMagicShield) * atPoisonMagicShieldPercent / 1024) *
              (1024 - atAllShieldIgnorePercent) / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fMagicShieldParam * GlobalParam::levelCof(atLevel) + res));
    return res > 768 ? 768 : res;
}

// 无双
int ChAttr::getStrain() const {
    return atStrainRate +
           static_cast<int>(
               (atStrainBase + atStrainBase * atStrainPercent / 1024) *
               1024 / (GlobalParam::get().fInsightParam * GlobalParam::levelCof(atLevel))
           );
}
// 急速
int ChAttr::getHaste() const {
    int res = atHasteBasePercentAdd +
              static_cast<int>(
                  atHasteBase *
                  1024 / (GlobalParam::get().fHasteRate * GlobalParam::levelCof(atLevel))
              );
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
