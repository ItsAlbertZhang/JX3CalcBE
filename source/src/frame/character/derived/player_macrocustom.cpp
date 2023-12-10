#include "frame/character/derived/player.h"

using namespace ns_frame;

static void constructorBefore(MacroCustom *self);
static void constructorAfter(MacroCustom *self);

MacroCustom::MacroCustom(const std::string &script) {
    constructorBefore(this);
    lua.script(script);
    constructorAfter(this);
}

MacroCustom::MacroCustom(const std::filesystem::path &scriptfile) {
    constructorBefore(this);
    lua.script_file(scriptfile.string());
    constructorAfter(this);
}

static void constructorBefore(MacroCustom *self) {
    self->lua.open_libraries(sol::lib::base);
    self->lua.new_usertype<CharacterAttr>(
        "CharacterAttr",
        "atVitalityBase", &CharacterAttr::atVitalityBase,
        "atStrengthBase", &CharacterAttr::atStrengthBase,
        "atAgilityBase", &CharacterAttr::atAgilityBase,
        "atSpiritBase", &CharacterAttr::atSpiritBase,
        "atSpunkBase", &CharacterAttr::atSpunkBase,
        "atPhysicsAttackPowerBase", &CharacterAttr::atPhysicsAttackPowerBase,
        "atSolarAttackPowerBase", &CharacterAttr::atSolarAttackPowerBase,
        "atLunarAttackPowerBase", &CharacterAttr::atLunarAttackPowerBase,
        "atNeutralAttackPowerBase", &CharacterAttr::atNeutralAttackPowerBase,
        "atPoisonAttackPowerBase", &CharacterAttr::atPoisonAttackPowerBase,
        "atTherapyPowerBase", &CharacterAttr::atTherapyPowerBase,
        "atPhysicsCriticalStrike", &CharacterAttr::atPhysicsCriticalStrike,
        "atSolarCriticalStrike", &CharacterAttr::atSolarCriticalStrike,
        "atLunarCriticalStrike", &CharacterAttr::atLunarCriticalStrike,
        "atNeutralCriticalStrike", &CharacterAttr::atNeutralCriticalStrike,
        "atPoisonCriticalStrike", &CharacterAttr::atPoisonCriticalStrike,
        "atPhysicsCriticalDamagePowerBase", &CharacterAttr::atPhysicsCriticalDamagePowerBase,
        "atSolarCriticalDamagePowerBase", &CharacterAttr::atSolarCriticalDamagePowerBase,
        "atLunarCriticalDamagePowerBase", &CharacterAttr::atLunarCriticalDamagePowerBase,
        "atNeutralCriticalDamagePowerBase", &CharacterAttr::atNeutralCriticalDamagePowerBase,
        "atPoisonCriticalDamagePowerBase", &CharacterAttr::atPoisonCriticalDamagePowerBase,
        "atPhysicsOvercomeBase", &CharacterAttr::atPhysicsOvercomeBase,
        "atSolarOvercomeBase", &CharacterAttr::atSolarOvercomeBase,
        "atLunarOvercomeBase", &CharacterAttr::atLunarOvercomeBase,
        "atNeutralOvercomeBase", &CharacterAttr::atNeutralOvercomeBase,
        "atPoisonOvercomeBase", &CharacterAttr::atPoisonOvercomeBase,
        "atSurplusValueBase", &CharacterAttr::atSurplusValueBase,
        "atStrainBase", &CharacterAttr::atStrainBase,
        "atHasteBase", &CharacterAttr::atHasteBase,
        "atPhysicsShieldBase", &CharacterAttr::atPhysicsShieldBase,
        "atSolarMagicShieldBase", &CharacterAttr::atSolarMagicShieldBase,
        "atLunarMagicShieldBase", &CharacterAttr::atLunarMagicShieldBase,
        "atNeutralMagicShieldBase", &CharacterAttr::atNeutralMagicShieldBase,
        "atPoisonMagicShieldBase", &CharacterAttr::atPoisonMagicShieldBase,
        "atDodge", &CharacterAttr::atDodge,
        "atParryBase", &CharacterAttr::atParryBase,
        "atParryValueBase", &CharacterAttr::atParryValueBase,
        "atToughnessBase", &CharacterAttr::atToughnessBase,
        "atDecriticalDamagePowerBase", &CharacterAttr::atDecriticalDamagePowerBase,
        "atMeleeWeaponDamageBase", &CharacterAttr::atMeleeWeaponDamageBase,
        "atMeleeWeaponDamageRand", &CharacterAttr::atMeleeWeaponDamageRand
        // lua.new_usertype<CharacterAttr> end
    );
    self->lua.new_usertype<Player>(
        "Player",
        "chAttr", &Player::chAttr,
        "characterGetSelect", &Player::characterGetSelect,
        "characterGetTargetID", &Player::characterGetTargetID,
        "buffExist", &Player::buffExist,
        "buffGet", &Player::buffGet,
        "buffGetByOwner", &Player::buffGetByOwner,
        "buffAdd", sol::overload(&Player::buffAdd4, &Player::buffAdd5, &Player::buffAdd7),
        "buffDel", &Player::buffDel,
        "buffDelGroup", &Player::buffDelGroup,
        "buffDelMultiGroupByID", &Player::buffDelMultiGroupByID,
        "buffSetLeftActiveCount", &Player::buffSetLeftActiveCount,
        "buffSetNextActiveFrame", &Player::buffSetNextActiveFrame,
        "cooldownClearTime", &Player::cooldownClearTime,
        "cooldownModify", &Player::cooldownModify,
        "cooldownReset", &Player::cooldownReset,
        "skillGetLevel", &Player::skillGetLevel,
        "skillCast", sol::overload(&Player::skillCast2, &Player::skillCast3, &Player::skillCast4),
        "skillCastXYZ", &Player::skillCastXYZ,
        "skillrecipeExist", &Player::skillrecipeExist,
        "sceneGet", &Player::sceneGet,
        "timerSet", sol::overload(&Player::timerSet3, &Player::timerSet4),
        "otherIsFormationLeader", &Player::otherIsFormationLeader,
        "otherIsInParty", &Player::otherIsInParty,
        "otherGetKungfuMountID", &Player::otherGetKungfuMountID,
        "otherGetMapID", &Player::otherGetMapID,
        "otherClearAllNpcThreat", &Player::otherClearAllNpcThreat,
        "otherCreatePublicShadow", &Player::otherCreatePublicShadow,
        "otherDestroyPublicShadow", &Player::otherDestroyPublicShadow,
        "otherDoAction", &Player::otherDoAction,
        "otherPlayPublicShadowAnimation", &Player::otherPlayPublicShadowAnimation,
        "dwID", &Player::dwID,
        "nLevel", &Player::nLevel,
        "nX", &Player::nX, "nY", &Player::nY, "nZ", &Player::nZ,
        "nRoleType", &Player::nRoleType,
        "nCurrentSunEnergy", &Player::nCurrentSunEnergy,
        "nCurrentMoonEnergy", &Player::nCurrentMoonEnergy,
        "nSunPowerValue", &Player::nSunPowerValue,
        "nMoonPowerValue", &Player::nMoonPowerValue,
        "bSurplusAutoCast", &Player::bSurplusAutoCast,
        "bSurplusAutoReplenish", &Player::bSurplusAutoReplenish,
        "bFightState", &Player::bFightState,
        "fMaxLife64", &Player::fMaxLife64,
        "fCurrentLife64", &Player::fCurrentLife64,
        "dwKungfuID", &Player::dwKungfuID,
        "attrImportFromJX3BOX", &Player::attrImportFromJX3BOX,
        "cast", &Player::cast,
        "skillActive", &Player::skillActive,
        "skillDeactive", &Player::skillDeactive,
        "skillLearn", &Player::skillLearn,
        "skillrecipeAdd", &Player::skillrecipeAdd,
        "skillrecipeRemove", &Player::skillrecipeRemove,
        "skilleventAdd", &Player::skilleventAdd,
        "skilleventRemove", &Player::skilleventRemove,
        "publicCooldownID", &Player::publicCooldownID,
        "delayBase", &Player::delayBase,
        "delayRand", &Player::delayRand,
        "delayCustom", &Player::delayCustom,
        "macroIdx", &Player::macroIdx
        // lua.new_usertype<Player> end
    );
}

static void constructorAfter(MacroCustom *self) {
    self->attrInit = self->lua["AttrInit"];
    self->macroPrepare = self->lua["MacroPrepare"];
    int macroNum = self->lua["MacroNum"].get<int>();
    for (int i = 0; i < macroNum; i++) {
        self->macroRuntime.push_back(self->lua["Macro" + std::to_string(i)]);
    }
}