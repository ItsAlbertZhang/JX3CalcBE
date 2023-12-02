#include "frame/static_lua.h"
#include "frame/character/character.h"
#include "frame/global/skill.h"
#include "frame/static_ref.h"
#include "frame/static_refmap.h"

using namespace ns_frame;

const std::vector<std::string> ns_framestatic::luaFuncStaticToDynamic = {
    // Skill
    "AddAttribute",
    "AddSlowCheckSelfBuff",
    "AddSlowCheckDestBuff",
    "AddSlowCheckSelfOwnBuff",
    "AddSlowCheckDestOwnBuff",
    "AddCheckSelfLearntSkill",
    "BindBuff",
    "SetPublicCoolDown",
    "SetNormalCoolDown",
    "SetCheckCoolDown",
    "SetSubsectionSkill",
    "SetSunSubsectionSkill",
    "SetMoonSubsectionSkill",
    // Character
    "CastSkill",
    "AddBuff",
    "DelBuff",
    "IsHaveBuff",
    "ModifyCoolDown",
    "GetBuff",
    "GetSkillLevel",
    "SetTimer",
    "GetSkillTarget",
    "GetKungfuMountID",
    "IsFormationLeader",
    "CastSkillXYZ",
    "PlayPublicShadowAnimation",
};

bool ns_framestatic::luaInit(sol::state &lua) {
    lua.new_usertype<Skill>("Skill",
                            "dwLevel", &Skill::dwLevel,
                            "nChannelInterval", &Skill::nChannelInterval,

                            "AddAttribute", sol::overload(&Skill::AddAttribute_iiii, &Skill::AddAttribute_iisi, &Skill::AddAttribute_iidi),

                            "AddSlowCheckSelfBuff", &Skill::AddSlowCheckSelfBuff,
                            "AddSlowCheckDestBuff", &Skill::AddSlowCheckDestBuff,
                            "AddSlowCheckSelfOwnBuff", &Skill::AddSlowCheckSelfOwnBuff,
                            "AddSlowCheckDestOwnBuff", &Skill::AddSlowCheckDestOwnBuff,

                            "AddCheckSelfLearntSkill", &Skill::AddCheckSelfLearntSkill,

                            "BindBuff", &Skill::BindBuff,

                            "SetPublicCoolDown", &Skill::SetPublicCoolDown,
                            "SetNormalCoolDown", &Skill::SetNormalCoolDown,
                            "SetCheckCoolDown", &Skill::SetCheckCoolDown,

                            "dwLevelUpExp", &Skill::dwLevelUpExp,
                            "nExpAddOdds", &Skill::nExpAddOdds,
                            "nPlayerLevelLimit", &Skill::nPlayerLevelLimit,

                            "nBaseThreat", &Skill::nBaseThreat,

                            "nCostLife", &Skill::nCostLife,
                            "nCostMana", &Skill::nCostMana,
                            "nCostStamina", &Skill::nCostStamina,
                            "nCostItemType", &Skill::nCostItemType,
                            "nCostItemIndex", &Skill::nCostItemIndex,
                            "nCostSprintPower", &Skill::nCostSprintPower,

                            "bIsAccumulate", &Skill::bIsAccumulate,

                            "SetSubsectionSkill", &Skill::SetSubsectionSkill,
                            "nChainBranch", &Skill::nChainBranch,
                            "nChainDepth", &Skill::nChainDepth,

                            "nMinRadius", &Skill::nMinRadius,
                            "nMaxRadius", &Skill::nMaxRadius,

                            "nProtectRadius", &Skill::nProtectRadius,
                            "nHeight", &Skill::nHeight,
                            "nRectWidth", &Skill::nRectWidth,
                            "nAngleRange", &Skill::nAngleRange,
                            "bFullAngleInAir", &Skill::bFullAngleInAir,
                            "nAreaRadius", &Skill::nAreaRadius,
                            "nTargetCountLimit", &Skill::nTargetCountLimit,
                            "bIgnorePrepareState", &Skill::bIgnorePrepareState,

                            "nPrepareFrames", &Skill::nPrepareFrames,
                            "nChannelFrame", &Skill::nChannelFrame,
                            "nBulletVelocity", &Skill::nBulletVelocity,

                            "bIsSunMoonPower", &Skill::bIsSunMoonPower,
                            "SetSunSubsectionSkill", &Skill::SetSunSubsectionSkill,
                            "SetMoonSubsectionSkill", &Skill::SetMoonSubsectionSkill,
                            "bIsFormationSkill", &Skill::bIsFormationSkill,
                            "nFormationRange", &Skill::nFormationRange,
                            "nLeastFormationPopulation", &Skill::nLeastFormationPopulation,

                            "nTargetLifePercentMin", &Skill::nTargetLifePercentMin,
                            "nTargetLifePercentMax", &Skill::nTargetLifePercentMax,

                            "nSelfLifePercentMin", &Skill::nSelfLifePercentMin,
                            "nSelfLifePercentMax", &Skill::nSelfLifePercentMax,

                            "nBeatBackRate", &Skill::nBeatBackRate,
                            "nBrokenRate", &Skill::nBrokenRate,
                            "nBreakRate", &Skill::nBreakRate,
                            "nDismountingRate", &Skill::nDismountingRate,

                            "nWeaponDamagePercent", &Skill::nWeaponDamagePercent);

    lua.new_usertype<Character>("Skill",
                                "CastSkill", sol::overload(&Character::CastSkill2, &Character::CastSkill4),
                                "AddBuff", &Character::AddBuff,
                                "DelBuff", &Character::DelBuff,
                                "IsHaveBuff", &Character::IsHaveBuff,
                                "ModifyCoolDown", &Character::ModifyCoolDown,
                                "GetBuff", &Character::GetBuff,
                                "GetSkillLevel", &Character::GetSkillLevel,
                                "SetTimer", sol::overload(&Character::SetTimer3, &Character::SetTimer4),
                                "GetSkillTarget", &Character::GetSkillTarget,
                                "GetKungfuMountID", &Character::GetKungfuMountID,
                                "IsFormationLeader", &Character::IsFormationLeader,
                                "CastSkillXYZ", &Character::CastSkillXYZ,
                                "PlayPublicShadowAnimation", &Character::PlayPublicShadowAnimation,
                                "dwID", &Character::dwID,
                                "nLevel", &Character::nLevel,
                                "nX", &Character::nX, "nY", &Character::nY, "nZ", &Character::nZ,
                                "nCurrentSunEnergy", &Character::nCurrentSunEnergy,
                                "nCurrentMoonEnergy", &Character::nCurrentMoonEnergy,
                                "nSunPowerValue", &Character::nSunPowerValue,
                                "nMoonPowerValue", &Character::nMoonPowerValue,
                                "bSurplusAutoCast", &Character::bSurplusAutoCast,
                                "bSurplusAutoReplenish", &Character::bSurplusAutoReplenish);

    lua.set_function("Include", LuaGlobalFunction::Include);
    lua.set_function("GetPlayer", LuaGlobalFunction::GetPlayer);
    lua.set_function("GetNpc", LuaGlobalFunction::GetNpc);
    lua.set_function("IsPlayer", LuaGlobalFunction::IsPlayer);
    lua.set_function("AdditionalAttribute", LuaGlobalFunction::AdditionalAttribute);

    sol::table AttributeType = lua.create_table();
    for (int i = 0; i < static_cast<int>(ns_framestatic::enumLuaAttributeType::COUNT); i++) {
        AttributeType[ns_framestatic::refLuaAttributeType[i]] = i;
    }
    lua["ATTRIBUTE_TYPE"] = AttributeType;

    sol::table AttributeEffectMode = lua.create_table();
    for (int i = 0; i < static_cast<int>(ns_framestatic::enumLuaAttributeEffectMode::COUNT); i++) {
        AttributeEffectMode[ns_framestatic::refLuaAttributeEffectMode[i]] = i;
    }
    lua["ATTRIBUTE_EFFECT_MODE"] = AttributeEffectMode;

    sol::table BuffCompareFlag = lua.create_table();
    for (int i = 0; i < static_cast<int>(ns_framestatic::enumLuaBuffCompareFlag::COUNT); i++) {
        BuffCompareFlag[ns_framestatic::refLuaBuffCompareFlag[i]] = i;
    }
    lua["BUFF_COMPARE_FLAG"] = BuffCompareFlag;

    sol::table SKILL_COMPARE_FLAG = lua.create_table();
    for (int i = 0; i < static_cast<int>(ns_framestatic::enumLuaSkillCompareFlag::COUNT); i++) {
        SKILL_COMPARE_FLAG[ns_framestatic::refLuaSkillCompareFlag[i]] = i;
    }
    lua["SKILL_COMPARE_FLAG"] = SKILL_COMPARE_FLAG;

    sol::table TARGET = lua.create_table();
    for (int i = 0; i < static_cast<int>(ns_framestatic::enumLuaTarget::COUNT); i++) {
        TARGET[ns_framestatic::refLuaTarget[i]] = i;
    }
    lua["TARGET"] = TARGET;

    sol::table SKILL_KIND_TYPE = lua.create_table();
    for (int i = 0; i < static_cast<int>(ns_framestatic::enumLuaSkillKindType::COUNT); i++) {
        SKILL_KIND_TYPE[ns_framestatic::refLuaSkillKindType[i]] = i;
    }
    lua["SKILL_KIND_TYPE"] = SKILL_KIND_TYPE;

    sol::table MOVE_STATE = lua.create_table();
    for (int i = 0; i < static_cast<int>(ns_framestatic::enumLuaMoveState::COUNT); i++) {
        MOVE_STATE[ns_framestatic::refLuaMoveState[i]] = i;
    }
    lua["MOVE_STATE"] = MOVE_STATE;

    lua["CONSUME_BASE"] = 100;
    lua["LENGTH_BASE"] = 64;

    return true;
}

void ns_framestatic::LuaGlobalFunction::Include(const std::string &filename) {
    return;
}

Character *ns_framestatic::LuaGlobalFunction::GetPlayer(int nCharacterID) {
    return Character::getCharacter(nCharacterID);
}

Character *ns_framestatic::LuaGlobalFunction::GetNpc(int nCharacterID) {
    return Character::getCharacter(nCharacterID);
}

bool ns_framestatic::LuaGlobalFunction::IsPlayer(int nCharacterID) {
    return Character::getCharacter(nCharacterID)->isPlayer;
}

void ns_framestatic::LuaGlobalFunction::AdditionalAttribute(Skill &skill) {
    // 出现于 Skill.lh, 其本应被 Include 包含. 但 Include 实际上置空, 因此在此处实现相关逻辑.
    skill.AddAttribute_iiii(
        static_cast<int>(enumLuaAttributeEffectMode::EFFECT_TO_SELF_AND_ROLLBACK),
        static_cast<int>(enumLuaAttributeType::DECRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE),
        100,
        0);
}