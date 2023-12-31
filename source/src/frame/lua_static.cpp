#include "frame/lua_static.h"
#include "frame/character/character.h"
#include "frame/global/skill.h"
#include "frame/lua_runtime.h"
#include "frame/ref/lua_adaptive_type.h"
#include "frame/ref/lua_attribute_type.h"
#include "frame/ref/lua_other.h"
#include "global/log.h"
#include <random>
#include <sol/sol.hpp>

#define UNREFERENCED_PARAMETER(P) (P)

using namespace ns_frame;

const std::vector<std::string> ns_frame::luaFuncStaticToDynamic = {
    // Skill
    "SetDelaySubSkill",
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
    "GetBuffByOwner",
    "GetSkillLevel",
    "SetTimer",
    "GetSkillTarget",
    "GetKungfuMountID",
    "IsFormationLeader",
    "CastSkillXYZ",
    "PlayPublicShadowAnimation",
    "GetMapID",
    "GetScene",
    "ClearAllNpcThreat",
    "ClearCDTime",
    "DelGroupBuff",
    "IsInParty",
    "DoAction",
    "ResetCD",
    "GetSelectCharacter",
    "IsSkillRecipeActive",
    "DelMultiGroupBuffByID",
    "DestroyPublicShadow",
    "CreatePublicShadow",
    "SetBuffLeftActiveCount",
    "SetBuffNextActiveFrame",
};

std::shared_ptr<sol::state> ns_frame::luaInit() {
    auto lua = LuaFunc::getLua();
    lua->open_libraries(sol::lib::base);
    lua->open_libraries(sol::lib::table);

    // clang-format off
    
    lua->new_usertype<Skill>(
        "Skill",
        "dwSkillID", &Skill::dwSkillID,
        "dwLevel", &Skill::dwLevel,
        "nChannelInterval", &Skill::nChannelInterval,

        "SetDelaySubSkill", &Skill::SetDelaySubSkill,
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

        "nWeaponDamagePercent", &Skill::nWeaponDamagePercent
    );

    lua->new_usertype<Character>(
        "Character",
        "GetSelectCharacter", &Character::characterGetSelect,
        "GetSkillTarget", &Character::characterGetTargetID,
        "IsHaveBuff", &Character::buffExist,
        "GetBuff", &Character::buffGet,
        "GetBuffByOwner", &Character::buffGetByOwner,
        "AddBuff", sol::overload(&Character::buffAdd4, &Character::buffAdd5, &Character::buffAdd7),
        "DelBuff", &Character::buffDel,
        "DelGroupBuff", &Character::buffDelGroup,
        "DelMultiGroupBuffByID", &Character::buffDelMultiGroupByID,
        "SetBuffLeftActiveCount", &Character::buffSetLeftActiveCount,
        "SetBuffNextActiveFrame", &Character::buffSetNextActiveFrame,
        "ClearCDTime", &Character::cooldownClearTime,
        "ModifyCoolDown", &Character::cooldownModify,
        "ResetCD", &Character::cooldownReset,
        "GetSkillLevel", &Character::skillGetLevel,
        "CastSkill", sol::overload(&Character::skillCast2, &Character::skillCast3, &Character::skillCast4),
        "CastSkillXYZ", &Character::skillCastXYZ,
        "IsSkillRecipeActive", &Character::skillrecipeExist,
        "GetScene", &Character::sceneGet,
        "SetTimer", sol::overload(&Character::timerSet3, &Character::timerSet4),
        "IsFormationLeader", &Character::otherIsFormationLeader,
        "IsInParty", &Character::otherIsInParty,
        "GetKungfuMountID", &Character::otherGetKungfuMountID,
        "GetMapID", &Character::otherGetMapID,
        "ClearAllNpcThreat", &Character::otherClearAllNpcThreat,
        "CreatePublicShadow", &Character::otherCreatePublicShadow,
        "DestroyPublicShadow", &Character::otherDestroyPublicShadow,
        "DoAction", &Character::otherDoAction,
        "PlayPublicShadowAnimation", &Character::otherPlayPublicShadowAnimation,
        "dwID", &Character::dwID,
        "nLevel", &Character::nLevel,
        "nX", &Character::nX, "nY", &Character::nY, "nZ", &Character::nZ,
        "nRoleType", &Character::nRoleType,
        "nTouchRange", &Character::nTouchRange,
        "nCurrentSunEnergy", &Character::nCurrentSunEnergy,
        "nCurrentMoonEnergy", &Character::nCurrentMoonEnergy,
        "nSunPowerValue", &Character::nSunPowerValue,
        "nMoonPowerValue", &Character::nMoonPowerValue,
        "bSurplusAutoCast", &Character::bSurplusAutoCast,
        "bSurplusAutoReplenish", &Character::bSurplusAutoReplenish,
        "bFightState", &Character::bFightState,
        "fMaxLife64", &Character::fMaxLife64,
        "fCurrentLife64", &Character::fCurrentLife64
    );

    lua->new_usertype<BuffItem>(
        "Buff",
        "nLevel", &BuffItem::nLevel,
        "nIndex", &BuffItem::nIndex,
        "nStackNum", &BuffItem::nStackNum,
        "nLeftActiveCount", &BuffItem::nLeftActiveCount,
        "nNextActiveFrame", &BuffItem::nNextActiveFrame,
        "nCustomValue", &BuffItem::nCustomValue
    );

    lua->new_usertype<ChScene>(
        "Scene",
        "nType", &ChScene::nType
    );

    // clang-format on

    lua->set_function("Include", LuaGlobalFunction::Include);
    lua->set_function("GetPlayer", LuaGlobalFunction::GetPlayer);
    lua->set_function("GetNpc", LuaGlobalFunction::GetNpc);
    lua->set_function("IsPlayer", LuaGlobalFunction::IsPlayer);
    lua->set_function("AdditionalAttribute", LuaGlobalFunction::AdditionalAttribute);
    lua->set_function("IsLangKeXingMap", LuaGlobalFunction::IsLangKeXingMap);
    lua->set_function("ModityCDToUI", LuaGlobalFunction::ModityCDToUI);
    lua->set_function("CheckInTongWar", LuaGlobalFunction::CheckInTongWar);
    lua->set_function("IsTreasureBattleFieldMap", LuaGlobalFunction::IsTreasureBattleFieldMap);
    lua->set_function("GetValueByBits", LuaGlobalFunction::GetValueByBits);
    lua->set_function("SetValueByBits", LuaGlobalFunction::SetValueByBits);
    lua->set_function("RemoteCallToClient", LuaGlobalFunction::RemoteCallToClient);
    lua->set_function("GetDistanceSq", LuaGlobalFunction::GetDistanceSq);
    lua->set_function("Random", LuaGlobalFunction::Random);

    (*lua)["CONSUME_BASE"]          = 100;
    (*lua)["LENGTH_BASE"]           = 64;
    (*lua)["MELEE_ATTACK_DISTANCE"] = 4 * 64;

    sol::table AttributeType = lua->create_table();
    for (int i = 0; i < static_cast<int>(ref::enumLuaAttributeType::COUNT); i++) {
        AttributeType[ref::refLuaAttributeType[i]] = i;
    }
    (*lua)["ATTRIBUTE_TYPE"] = AttributeType;

    sol::table AttributeEffectMode = lua->create_table();
    for (int i = 0; i < static_cast<int>(ref::enumLuaAttributeEffectMode::COUNT); i++) {
        AttributeEffectMode[ref::refLuaAttributeEffectMode[i]] = i;
    }
    (*lua)["ATTRIBUTE_EFFECT_MODE"] = AttributeEffectMode;

    sol::table BuffCompareFlag = lua->create_table();
    for (int i = 0; i < static_cast<int>(ref::enumLuaBuffCompareFlag::COUNT); i++) {
        BuffCompareFlag[ref::refLuaBuffCompareFlag[i]] = i;
    }
    (*lua)["BUFF_COMPARE_FLAG"] = BuffCompareFlag;

    sol::table SKILL_COMPARE_FLAG = lua->create_table();
    for (int i = 0; i < static_cast<int>(ref::enumLuaSkillCompareFlag::COUNT); i++) {
        SKILL_COMPARE_FLAG[ref::refLuaSkillCompareFlag[i]] = i;
    }
    (*lua)["SKILL_COMPARE_FLAG"] = SKILL_COMPARE_FLAG;

    sol::table TARGET = lua->create_table();
    for (int i = 0; i < static_cast<int>(ref::enumLuaTarget::COUNT); i++) {
        TARGET[ref::refLuaTarget[i]] = i;
    }
    (*lua)["TARGET"] = TARGET;

    sol::table SKILL_KIND_TYPE = lua->create_table();
    for (int i = 0; i < static_cast<int>(ref::enumLuaSkillKindType::COUNT); i++) {
        SKILL_KIND_TYPE[ref::refLuaSkillKindType[i]] = i;
    }
    (*lua)["SKILL_KIND_TYPE"] = SKILL_KIND_TYPE;

    sol::table MOVE_STATE = lua->create_table();
    for (int i = 0; i < static_cast<int>(ref::enumLuaMoveState::COUNT); i++) {
        MOVE_STATE[ref::refLuaMoveState[i]] = i;
    }
    (*lua)["MOVE_STATE"] = MOVE_STATE;

    sol::table ROLE_TYPE = lua->create_table();
    for (int i = 0; i < static_cast<int>(ref::enumLuaRoleType::COUNT); i++) {
        ROLE_TYPE[ref::refLuaRoleType[i]] = i;
    }
    (*lua)["ROLE_TYPE"] = ROLE_TYPE;

    sol::table KUNGFU_ADAPTIVETYPE_LIST = lua->create_table();
    for (auto &i : ref::mapLuaAdaptiveType) {
        sol::table sub_table                                = lua->create_table();
        sub_table["adaptiveType"]                           = static_cast<int>(i.second);
        KUNGFU_ADAPTIVETYPE_LIST[static_cast<int>(i.first)] = sub_table;
    }
    (*lua)["KUNGFU_ADAPTIVETYPE_LIST"] = KUNGFU_ADAPTIVETYPE_LIST;

    return lua;
}

void LuaGlobalFunction::Include(const std::string &filename) {
    UNREFERENCED_PARAMETER(filename);
    return;
}

Character *LuaGlobalFunction::GetPlayer(int nCharacterID) {
    return Character::characterGet(nCharacterID);
}

Character *LuaGlobalFunction::GetNpc(int nCharacterID) {
    return Character::characterGet(nCharacterID);
}

bool LuaGlobalFunction::IsPlayer(int nCharacterID) {
    return Character::characterGet(nCharacterID)->isPlayer;
}

void LuaGlobalFunction::AdditionalAttribute(Skill &skill) {
    // 出现于 Skill.lh, 其本应被 Include 包含. 但 Include 实际上置空, 因此在此处实现相关逻辑.
    skill.AddAttribute_iiii(
        static_cast<int>(ref::enumLuaAttributeEffectMode::EFFECT_TO_SELF_AND_ROLLBACK),
        static_cast<int>(ref::enumLuaAttributeType::DECRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE),
        100,
        0
    );
}

bool LuaGlobalFunction::IsLangKeXingMap(int mapID) {
    UNREFERENCED_PARAMETER(mapID);
    return false;
}

void LuaGlobalFunction::ModityCDToUI(ns_frame::Character *character, int skillID, int c, int d) {
    UNREFERENCED_PARAMETER(character);
    UNREFERENCED_PARAMETER(skillID);
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(d);
    return;
}

bool LuaGlobalFunction::CheckInTongWar(ns_frame::Character *character) {
    UNREFERENCED_PARAMETER(character);
    return false;
}

bool LuaGlobalFunction::IsTreasureBattleFieldMap(int mapID) {
    UNREFERENCED_PARAMETER(mapID);
    return false;
}

/* ```lua
-- 函数名	: CustomFunction.GetValueByBit
-- 函数描述	: 获得数字的某一比特位值
-- 参数列表	:  nValue：值；nBit：取值范围[0,31]
-- 返回值	: 1 or 0
-- 备注		: CustomFunction.GetValueByBit(10, 31)，表示获取数字“10”的第31个Bit位的值。
function CustomFunction.GetValueByBit(nValue, nBit)
    if nBit > 31 or nBit < 0 then
        print(">>>>>>>CustomFunction.GetValueByBit Arg ERROR!!!!!BitIndex error")
        --return nValue
    end
    return GetValueByBits(nValue, nBit, 1)
    --return math.floor(nValue / 2 ^ nBit) % 2
end
``` */
int LuaGlobalFunction::GetValueByBits(int nValue, int nBit, int c) {
    UNREFERENCED_PARAMETER(c);
    if (nBit > 31 || nBit < 0) {
        LOG_ERROR(">>>>>>>CustomFunction.GetValueByBit Arg ERROR!!!!!BitIndex error{}", "");
    }
    return (nValue >> nBit) & 1;
}

/* ```lua
-- 函数名	: CustomFunction.SetValueByBit
-- 函数描述	: 将某个值的某一比特位值设置为0或者1
-- 参数列表	:  nValue：值；nBit：取值范围[0,31]；nNewBit：只能为0或者1
-- 返回值	: 设置完bit之后的新值
-- 备注		: CustomFunction.SetValueByBit(10,31,1)，表示将数字“10”的第31个Bit位的值置为1
function CustomFunction.SetValueByBit(nValue, nBit, nNewBitValue)
    if nNewBitValue > 1 or nNewBitValue < 0 then
        print(">>>>>>>CustomFunction.SetValueByBit Arg ERROR!!!!!nNewBit Must be 0 or 1,")
        return nValue
    end
    if nBit > 31 or nBit < 0 then
        print(">>>>>>>CustomFunction.SetValueByBit Arg ERROR!!!!!BitIndex error")
        return nValue
    end
    return SetValueByBits(nValue, nBit, 1, nNewBitValue)
    --如果要设置的新值和旧值一样，那么值不变，直接返回
    --if math.floor(nValue / 2 ^ nBit) % 2 == nNewBitValue then
        --return nValue
    --end
    --如果参数不正确那么
    --if nNewBitValue > 1 or nNewBitValue < 0 or nBit > 31 or nBit < 0 then
        --print(">>>>>>>CustomFunction.SetValueByBit Arg ERROR!!!!!nBit=[] nNewBit Must be 0 or 1,")
        --return nValue
    --end
    --0设成1加值，1变成0减值
    --print("SetValueByBit="..nNewBitValue)
    --if nNewBitValue == 1 then
        --return nValue + 2 ^ nBit
    --else
        --return nValue - 2 ^ nBit
    --end
end
``` */
int LuaGlobalFunction::SetValueByBits(int nValue, int nBit, int c, int nNewBitValue) {
    UNREFERENCED_PARAMETER(c);
    if (nNewBitValue > 1 || nNewBitValue < 0) {
        LOG_ERROR(">>>>>>>CustomFunction.SetValueByBit Arg ERROR!!!!!nNewBit Must be 0 or 1,{}", "");
        return nValue;
    }
    if (nBit > 31 || nBit < 0) {
        LOG_ERROR(">>>>>>>CustomFunction.SetValueByBit Arg ERROR!!!!!BitIndex error{}", "");
        return nValue;
    }
    return (nValue & ~(1 << nBit)) | (nNewBitValue << nBit);
}

void LuaGlobalFunction::RemoteCallToClient() {
    return;
}

int LuaGlobalFunction::GetDistanceSq(int pX, int pY, int pZ, int tX, int tY, int tZ) {
    return (pX - tX) * (pX - tX) + (pY - tY) * (pY - tY) + (pZ - tZ) * (pZ - tZ);
}

int LuaGlobalFunction::Random(int min, int max) {
    std::random_device              rd;
    std::mt19937                    gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return static_cast<int>(dis(gen));
}
