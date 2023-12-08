#include "frame/character/property/attribute.h"
#include "frame/globalparam.h"

using namespace ns_frame;

// 主属性
int CharacterAttr::getVitality() const {
    return atBasePotentialAdd +
           atVitalityBase +
           atVitalityBase * atVitalityBasePercentAdd / 1024;
}
int CharacterAttr::getStrength() const {
    return atBasePotentialAdd +
           atStrengthBase +
           atStrengthBase * atStrengthBasePercentAdd / 1024;
}
int CharacterAttr::getAgility() const {
    return atBasePotentialAdd +
           atAgilityBase +
           atAgilityBase * atAgilityBasePercentAdd / 1024;
}
int CharacterAttr::getSpirit() const {
    return atBasePotentialAdd +
           atSpiritBase +
           atSpiritBase * atSpiritBasePercentAdd / 1024;
}
int CharacterAttr::getSpunk() const {
    return atBasePotentialAdd +
           atSpunkBase +
           atSpunkBase * atSpunkBasePercentAdd / 1024;
}

// 攻击力
int CharacterAttr::getPhysicsAttackPower() const {
    return atPhysicsAttackPowerBase +
           atPhysicsAttackPowerBase * atPhysicsAttackPowerPercent / 1024;
}
int CharacterAttr::getSolarAttackPower() const {
    return (atSolarAttackPowerBase + atMagicAttackPowerBase) +
           (atSolarAttackPowerBase + atMagicAttackPowerBase) * (atSolarAttackPowerPercent + atMagicAttackPowerPercent) / 1024 +
           atSpunkToSolarAndLunarAttackPowerCof * getSpunk() / 1024;
}
int CharacterAttr::getLunarAttackPower() const {
    return (atLunarAttackPowerBase + atMagicAttackPowerBase) +
           (atLunarAttackPowerBase + atMagicAttackPowerBase) * (atLunarAttackPowerPercent + atMagicAttackPowerPercent) / 1024 +
           atSpunkToSolarAndLunarAttackPowerCof * getSpunk() / 1024;
}
int CharacterAttr::getNeutralAttackPower() const {
    return (atNeutralAttackPowerBase + atMagicAttackPowerBase) +
           (atNeutralAttackPowerBase + atMagicAttackPowerBase) * (atNeutralAttackPowerPercent + atMagicAttackPowerPercent) / 1024;
}
int CharacterAttr::getPoisonAttackPower() const {
    return (atPoisonAttackPowerBase + atMagicAttackPowerBase) +
           (atPoisonAttackPowerBase + atMagicAttackPowerBase) * (atPoisonAttackPowerPercent + atMagicAttackPowerPercent) / 1024;
}

// 会心
int CharacterAttr::getPhysicsCriticalStrike() const {
    return static_cast<int>((atPhysicsCriticalStrike + atAllTypeCriticalStrike +
                             atPhysicsCriticalStrikeAdd) *
                            10000 / (GlobalParam::get().fCriticalStrikeParam * GlobalParam::levelCof(atLevel))) +
           atPhysicsCriticalStrikeBaseRate;
}
int CharacterAttr::getSolarCriticalStrike() const {
    return static_cast<int>((atSolarCriticalStrike + atAllTypeCriticalStrike + atMagicCriticalStrike +
                             atSolarCriticalStrikeAdd + atSpunkToSolarAndLunarCriticalStrikeCof * getSpunk() / 1024) *
                            10000 / (GlobalParam::get().fCriticalStrikeParam * GlobalParam::levelCof(atLevel))) +
           atSolarCriticalStrikeBaseRate;
}
int CharacterAttr::getLunarCriticalStrike() const {
    return static_cast<int>((atLunarCriticalStrike + atAllTypeCriticalStrike + atMagicCriticalStrike +
                             atLunarCriticalStrikeAdd + atSpunkToSolarAndLunarCriticalStrikeCof * getSpunk() / 1024) *
                            10000 / (GlobalParam::get().fCriticalStrikeParam * GlobalParam::levelCof(atLevel))) +
           atLunarCriticalStrikeBaseRate;
}
int CharacterAttr::getNeutralCriticalStrike() const {
    return static_cast<int>((atNeutralCriticalStrike + atAllTypeCriticalStrike + atMagicCriticalStrike +
                             atNeutralCriticalStrikeAdd) *
                            10000 / (GlobalParam::get().fCriticalStrikeParam * GlobalParam::levelCof(atLevel))) +
           atNeutralCriticalStrikeBaseRate;
}
int CharacterAttr::getPoisonCriticalStrike() const {
    return static_cast<int>((atPoisonCriticalStrike + atAllTypeCriticalStrike + atMagicCriticalStrike +
                             atPoisonCriticalStrikeAdd) *
                            10000 / (GlobalParam::get().fCriticalStrikeParam * GlobalParam::levelCof(atLevel))) +
           atPoisonCriticalStrikeBaseRate;
}

// 会心效果
int CharacterAttr::getPhysicsCriticalDamagePower() const {
    int res = static_cast<int>((atAllTypeCriticalDamagePowerBase + atPhysicsCriticalDamagePowerBase) *
                               1024 / (GlobalParam::get().fCriticalStrikePowerParam * GlobalParam::levelCof(atLevel))) +
              atPhysicsCriticalDamagePowerBaseKiloNumRate;
    return res > 1280 ? 1280 : res;
}
int CharacterAttr::getSolarCriticalDamagePower() const {
    int res = static_cast<int>((atSolarCriticalDamagePowerBase + atAllTypeCriticalDamagePowerBase + atMagicCriticalDamagePowerBase) *
                               1024 / (GlobalParam::get().fCriticalStrikePowerParam * GlobalParam::levelCof(atLevel))) +
              atSolarCriticalDamagePowerBaseKiloNumRate + atMagicCriticalDamagePowerBaseKiloNumRate;
    return res > 1280 ? 1280 : res;
}
int CharacterAttr::getLunarCriticalDamagePower() const {
    int res = static_cast<int>((atLunarCriticalDamagePowerBase + atAllTypeCriticalDamagePowerBase + atMagicCriticalDamagePowerBase) *
                               1024 / (GlobalParam::get().fCriticalStrikePowerParam * GlobalParam::levelCof(atLevel))) +
              atLunarCriticalDamagePowerBaseKiloNumRate + atMagicCriticalDamagePowerBaseKiloNumRate;
    return res > 1280 ? 1280 : res;
}
int CharacterAttr::getNeutralCriticalDamagePower() const {
    int res = static_cast<int>((atNeutralCriticalDamagePowerBase + atAllTypeCriticalDamagePowerBase + atMagicCriticalDamagePowerBase) *
                               1024 / (GlobalParam::get().fCriticalStrikePowerParam * GlobalParam::levelCof(atLevel))) +
              atNeutralCriticalDamagePowerBaseKiloNumRate + atMagicCriticalDamagePowerBaseKiloNumRate;
    return res > 1280 ? 1280 : res;
}
int CharacterAttr::getPoisonCriticalDamagePower() const {
    int res = static_cast<int>((atPoisonCriticalDamagePowerBase + atAllTypeCriticalDamagePowerBase + atMagicCriticalDamagePowerBase) *
                               1024 / (GlobalParam::get().fCriticalStrikePowerParam * GlobalParam::levelCof(atLevel))) +
              atPoisonCriticalDamagePowerBaseKiloNumRate + atMagicCriticalDamagePowerBaseKiloNumRate;
    return res > 1280 ? 1280 : res;
}

// 破防
int CharacterAttr::getPhysicsOvercome() const {
    return static_cast<int>((atPhysicsOvercomeBase +
                             atPhysicsOvercomeBase * atPhysicsOvercomePercent / 1024) *
                            1024 / (GlobalParam::get().fOvercomeParam * GlobalParam::levelCof(atLevel)));
}
int CharacterAttr::getSolarOvercome() const {
    return static_cast<int>(((atSolarOvercomeBase + atMagicOvercome) +
                             (atSolarOvercomeBase + atMagicOvercome) * atSolarOvercomePercent / 1024) *
                            1024 / (GlobalParam::get().fOvercomeParam * GlobalParam::levelCof(atLevel)));
}
int CharacterAttr::getLunarOvercome() const {
    return static_cast<int>(((atLunarOvercomeBase + atMagicOvercome) +
                             (atLunarOvercomeBase + atMagicOvercome) * atLunarOvercomePercent / 1024) *
                            1024 / (GlobalParam::get().fOvercomeParam * GlobalParam::levelCof(atLevel)));
}
int CharacterAttr::getNeutralOvercome() const {
    return static_cast<int>(((atNeutralOvercomeBase + atMagicOvercome) +
                             (atNeutralOvercomeBase + atMagicOvercome) * atNeutralOvercomePercent / 1024) *
                            1024 / (GlobalParam::get().fOvercomeParam * GlobalParam::levelCof(atLevel)));
}
int CharacterAttr::getPoisonOvercome() const {
    return static_cast<int>(((atPoisonOvercomeBase + atMagicOvercome) +
                             (atPoisonOvercomeBase + atMagicOvercome) * atPoisonOvercomePercent / 1024) *
                            1024 / (GlobalParam::get().fOvercomeParam * GlobalParam::levelCof(atLevel)));
}

// 防御
int CharacterAttr::getPhysicsShield(int atAllShieldIgnorePercent) const {
    int res = (atPhysicsShieldBase +
               atPhysicsShieldBase * atPhysicsShieldPercent / 1024 +
               atPhysicsShieldAdditional) *
              (1024 - atAllShieldIgnorePercent) / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fPhysicsShieldParam * GlobalParam::levelCof(atLevel) + res));
    return res > 768 ? 768 : res;
}
int CharacterAttr::getSolarShield(int atAllShieldIgnorePercent) const {
    int res = ((atSolarMagicShieldBase + atMagicShield) +
               (atSolarMagicShieldBase + atMagicShield) * atSolarMagicShieldPercent / 1024) *
              (1024 - atAllShieldIgnorePercent) / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fMagicShieldParam * GlobalParam::levelCof(atLevel) + res));
    return res > 768 ? 768 : res;
}
int CharacterAttr::getLunarShield(int atAllShieldIgnorePercent) const {
    int res = ((atLunarMagicShieldBase + atMagicShield) +
               (atLunarMagicShieldBase + atMagicShield) * atLunarMagicShieldPercent / 1024) *
              (1024 - atAllShieldIgnorePercent) / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fMagicShieldParam * GlobalParam::levelCof(atLevel) + res));
    return res > 768 ? 768 : res;
}
int CharacterAttr::getNeutralShield(int atAllShieldIgnorePercent) const {
    int res = ((atNeutralMagicShieldBase + atMagicShield) +
               (atNeutralMagicShieldBase + atMagicShield) * atNeutralMagicShieldPercent / 1024) *
              (1024 - atAllShieldIgnorePercent) / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fMagicShieldParam * GlobalParam::levelCof(atLevel) + res));
    return res > 768 ? 768 : res;
}
int CharacterAttr::getPoisonShield(int atAllShieldIgnorePercent) const {
    int res = ((atPoisonMagicShieldBase + atMagicShield) +
               (atPoisonMagicShieldBase + atMagicShield) * atPoisonMagicShieldPercent / 1024) *
              (1024 - atAllShieldIgnorePercent) / 1024;
    res = static_cast<int>(res * 1024 / (GlobalParam::get().fMagicShieldParam * GlobalParam::levelCof(atLevel) + res));
    return res > 768 ? 768 : res;
}

// 无双
int CharacterAttr::getStrain() const {
    return static_cast<int>((atStrainBase + atStrainBase * atStrainPercent / 1024) *
                            1024 / (GlobalParam::get().fInsightParam * GlobalParam::levelCof(atLevel))) +
           atStrainRate;
}
// 急速
int CharacterAttr::getHaste() const {
    int res = static_cast<int>(atHasteBase * 1024 / (GlobalParam::get().fHasteRate * GlobalParam::levelCof(atLevel))) + atHasteBasePercentAdd;
    res = res > 256 ? 256 : res;
    return res + atUnlimitHasteBasePercentAdd;
}
// 破招
int CharacterAttr::getSurplus() const {
    return static_cast<int>(atSurplusValueBase * GlobalParam::get().fSurplusParam);
}

// 伤害加成
int CharacterAttr::getPhysicsDamageAddPercent() const {
    return atAllDamageAddPercent;
}
int CharacterAttr::getSolarDamageAddPercent() const {
    return atAllDamageAddPercent + atAllMagicDamageAddPercent;
}
int CharacterAttr::getLunarDamageAddPercent() const {
    return atAllDamageAddPercent + atAllMagicDamageAddPercent;
}
int CharacterAttr::getNeutralDamageAddPercent() const {
    return atAllDamageAddPercent + atAllMagicDamageAddPercent;
}
int CharacterAttr::getPoisonDamageAddPercent() const {
    return atAllDamageAddPercent + atAllMagicDamageAddPercent;
}