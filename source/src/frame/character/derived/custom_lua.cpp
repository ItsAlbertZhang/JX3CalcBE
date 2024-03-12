#include "frame/character/derived/player.h"

using namespace ns_frame;

static void constructorBefore(CustomLua *self);
static void constructorAfter(CustomLua *self);

CustomLua::CustomLua(const std::string &script) {
    constructorBefore(this);
    lua.script(script);
    constructorAfter(this);
}

static void constructorBefore(CustomLua *self) {
    self->lua.open_libraries(sol::lib::base);

    auto chAttr = self->lua.new_usertype<ChAttr>("ChAttr");

    chAttr["atVitalityBase"]                   = &ChAttr::atVitalityBase;
    chAttr["atStrengthBase"]                   = &ChAttr::atStrengthBase;
    chAttr["atAgilityBase"]                    = &ChAttr::atAgilityBase;
    chAttr["atSpiritBase"]                     = &ChAttr::atSpiritBase;
    chAttr["atSpunkBase"]                      = &ChAttr::atSpunkBase;
    chAttr["atPhysicsAttackPowerBase"]         = &ChAttr::atPhysicsAttackPowerBase;
    chAttr["atSolarAttackPowerBase"]           = &ChAttr::atSolarAttackPowerBase;
    chAttr["atLunarAttackPowerBase"]           = &ChAttr::atLunarAttackPowerBase;
    chAttr["atNeutralAttackPowerBase"]         = &ChAttr::atNeutralAttackPowerBase;
    chAttr["atPoisonAttackPowerBase"]          = &ChAttr::atPoisonAttackPowerBase;
    chAttr["atTherapyPowerBase"]               = &ChAttr::atTherapyPowerBase;
    chAttr["atPhysicsCriticalStrike"]          = &ChAttr::atPhysicsCriticalStrike;
    chAttr["atSolarCriticalStrike"]            = &ChAttr::atSolarCriticalStrike;
    chAttr["atLunarCriticalStrike"]            = &ChAttr::atLunarCriticalStrike;
    chAttr["atNeutralCriticalStrike"]          = &ChAttr::atNeutralCriticalStrike;
    chAttr["atPoisonCriticalStrike"]           = &ChAttr::atPoisonCriticalStrike;
    chAttr["atPhysicsCriticalDamagePowerBase"] = &ChAttr::atPhysicsCriticalDamagePowerBase;
    chAttr["atSolarCriticalDamagePowerBase"]   = &ChAttr::atSolarCriticalDamagePowerBase;
    chAttr["atLunarCriticalDamagePowerBase"]   = &ChAttr::atLunarCriticalDamagePowerBase;
    chAttr["atNeutralCriticalDamagePowerBase"] = &ChAttr::atNeutralCriticalDamagePowerBase;
    chAttr["atPoisonCriticalDamagePowerBase"]  = &ChAttr::atPoisonCriticalDamagePowerBase;
    chAttr["atPhysicsOvercomeBase"]            = &ChAttr::atPhysicsOvercomeBase;
    chAttr["atSolarOvercomeBase"]              = &ChAttr::atSolarOvercomeBase;
    chAttr["atLunarOvercomeBase"]              = &ChAttr::atLunarOvercomeBase;
    chAttr["atNeutralOvercomeBase"]            = &ChAttr::atNeutralOvercomeBase;
    chAttr["atPoisonOvercomeBase"]             = &ChAttr::atPoisonOvercomeBase;
    chAttr["atSurplusValueBase"]               = &ChAttr::atSurplusValueBase;
    chAttr["atStrainBase"]                     = &ChAttr::atStrainBase;
    chAttr["atHasteBase"]                      = &ChAttr::atHasteBase;
    chAttr["atPhysicsShieldBase"]              = &ChAttr::atPhysicsShieldBase;
    chAttr["atSolarMagicShieldBase"]           = &ChAttr::atSolarMagicShieldBase;
    chAttr["atLunarMagicShieldBase"]           = &ChAttr::atLunarMagicShieldBase;
    chAttr["atNeutralMagicShieldBase"]         = &ChAttr::atNeutralMagicShieldBase;
    chAttr["atPoisonMagicShieldBase"]          = &ChAttr::atPoisonMagicShieldBase;
    chAttr["atDodge"]                          = &ChAttr::atDodge;
    chAttr["atParryBase"]                      = &ChAttr::atParryBase;
    chAttr["atParryValueBase"]                 = &ChAttr::atParryValueBase;
    chAttr["atToughnessBase"]                  = &ChAttr::atToughnessBase;
    chAttr["atDecriticalDamagePowerBase"]      = &ChAttr::atDecriticalDamagePowerBase;
    chAttr["atMeleeWeaponDamageBase"]          = &ChAttr::atMeleeWeaponDamageBase;
    chAttr["atMeleeWeaponDamageRand"]          = &ChAttr::atMeleeWeaponDamageRand;

    auto player = self->lua.new_usertype<Player>("Player");

    player["chAttr"]                 = &Player::chAttr;
    player["characterGetSelect"]     = &Player::characterGetSelect;
    player["characterGetTargetID"]   = &Player::characterGetTargetID;
    player["buffExist"]              = &Player::buffExist;
    player["buffGet"]                = &Player::buffGet;
    player["buffGetByOwner"]         = &Player::buffGetByOwner;
    player["buffAdd"]                = &Player::buffAddOptional;
    player["buffDel"]                = &Player::buffDel;
    player["buffDelGroup"]           = &Player::buffDelGroup;
    player["buffDelMultiGroupByID"]  = &Player::buffDelMultiGroupByID;
    player["buffSetLeftActiveCount"] = &Player::buffSetLeftActiveCount;
    player["buffSetNextActiveFrame"] = &Player::buffSetNextActiveFrame;
    player["cooldownClearTime"]      = &Player::cooldownClearTime;
    player["cooldownModify"]         = &Player::cooldownModify;
    player["cooldownReset"]          = &Player::cooldownReset;
    player["skillGetLevel"]          = &Player::skillGetLevel;
    player["skillCast"]              = sol::overload(
        static_cast<void (Character::*)(int, int)>(&Character::skillCast),
        static_cast<void (Character::*)(int, int, int)>(&Character::skillCast),
        static_cast<void (Character::*)(int, int, int, int)>(&Character::skillCast)
    );
    player["skillCastXYZ"]     = &Player::skillCastXYZ;
    player["skillrecipeExist"] = &Player::skillrecipeExist;
    player["sceneGet"]         = &Player::sceneGet;
    player["timerSet"]         = sol::overload(
        static_cast<void (Character::*)(int, std::string, int)>(&Character::timerSet),
        static_cast<void (Character::*)(int, std::string, int, int)>(&Character::timerSet)
    );
    player["otherIsFormationLeader"]         = &Player::otherIsFormationLeader;
    player["otherIsInParty"]                 = &Player::otherIsInParty;
    player["otherGetKungfuMountID"]          = &Player::otherGetKungfuMountID;
    player["otherGetMapID"]                  = &Player::otherGetMapID;
    player["otherClearAllNpcThreat"]         = &Player::otherClearAllNpcThreat;
    player["otherCreatePublicShadow"]        = &Player::otherCreatePublicShadow;
    player["otherDestroyPublicShadow"]       = &Player::otherDestroyPublicShadow;
    player["otherDoAction"]                  = &Player::otherDoAction;
    player["otherPlayPublicShadowAnimation"] = &Player::otherPlayPublicShadowAnimation;
    player["dwID"]                           = &Player::dwID;
    player["nLevel"]                         = &Player::nLevel;
    player["nX"]                             = &Player::nX;
    player["nY"]                             = &Player::nY;
    player["nZ"]                             = &Player::nZ;
    player["nRoleType"]                      = &Player::nRoleType;
    player["nCurrentSunEnergy"]              = &Player::nCurrentSunEnergy;
    player["nCurrentMoonEnergy"]             = &Player::nCurrentMoonEnergy;
    player["nSunPowerValue"]                 = &Player::nSunPowerValue;
    player["nMoonPowerValue"]                = &Player::nMoonPowerValue;
    player["bSurplusAutoCast"]               = &Player::bSurplusAutoCast;
    player["bSurplusAutoReplenish"]          = &Player::bSurplusAutoReplenish;
    player["bFightState"]                    = &Player::bFightState;
    player["fMaxLife64"]                     = &Player::fMaxLife64;
    player["fCurrentLife64"]                 = &Player::fCurrentLife64;
    player["attrImportFromData"]             = &Player::attrImportFromData;
    player["attrImportFromJX3BOX"]           = &Player::attrImportFromJX3BOX;
    player["buffTimeLeftTick"]               = &Player::buffTimeLeftTick;
    player["cast"]                           = &Player::cast;
    player["skillActive"]                    = &Player::skillActive;
    player["skillDeactive"]                  = &Player::skillDeactive;
    player["skillLearn"]                     = &Player::skillLearn;
    player["skillrecipeAdd"]                 = &Player::skillrecipeAdd;
    player["skillrecipeRemove"]              = &Player::skillrecipeRemove;
    player["skilleventAdd"]                  = &Player::skilleventAdd;
    player["skilleventRemove"]               = &Player::skilleventRemove;
    player["dwKungfuID"]                     = &Player::dwKungfuID;
    player["publicCooldownID"]               = &Player::publicCooldownID;
    player["delayBase"]                      = &Player::delayBase;
    player["delayRand"]                      = &Player::delayRand;
    player["delayCustom"]                    = &Player::delayCustom;
    player["macroIdx"]                       = &Player::macroIdx;
}

static void constructorAfter(CustomLua *self) {
    self->macroPrepare = self->lua["MacroPrepare"];
    int macroNum       = self->lua["MacroNum"].get<int>();
    for (int i = 0; i < macroNum; i++) {
        self->macroRuntime.push_back(self->lua["Macro" + std::to_string(i)]);
    }
}
