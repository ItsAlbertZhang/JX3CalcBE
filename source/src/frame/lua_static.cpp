#include "frame/lua_static.h"
#include "frame/character/character.h"
#include "frame/global/skill.h"
#include "frame/lua_runtime.h"
#include "frame/ref/lua_adaptive_type.h"
#include "frame/ref/lua_attribute_type.h"
#include "frame/ref/lua_other.h"
#include "plugin/log.h"
#include <optional>
#include <random>
#include <sol/sol.hpp>
#include <string>

#define UNREFERENCED_PARAMETER(P) (P)

using namespace jx3calc;
using namespace frame;

const char *const frame::luaFuncList[]{
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
const size_t frame::luaFuncListSize = sizeof(frame::luaFuncList) / sizeof(frame::luaFuncList[0]);

sol::state *frame::luaInit() {
    auto lua = LuaFunc::getLua();
    lua->open_libraries(sol::lib::base);
    lua->open_libraries(sol::lib::table);

    // lua 类型

    auto skill = lua->new_usertype<Skill>("Skill");

    skill["dwSkillID"]        = &Skill::dwSkillID;
    skill["dwLevel"]          = &Skill::dwLevel;
    skill["nChannelInterval"] = &Skill::nChannelInterval;
    skill["SetDelaySubSkill"] = &Skill::SetDelaySubSkill;
    skill["AddAttribute"]     = sol::overload(
        static_cast<void (Skill::*)(int, int, int, int)>(&Skill::AddAttribute),
        static_cast<void (Skill::*)(int, int, double, int)>(&Skill::AddAttribute),
        static_cast<void (Skill::*)(int, int, std::string, int)>(&Skill::AddAttribute),
        static_cast<void (Skill::*)(int, int, std::optional<char>, int)>(&Skill::AddAttribute)
        // std::optional<char> 其实是 nil
        // 注意需要保证 std::optional<char> 位于最后, 防止其优先级高于 std::string 而被先匹配
    );
    skill["AddSlowCheckSelfBuff"]      = &Skill::AddSlowCheckSelfBuff;
    skill["AddSlowCheckDestBuff"]      = &Skill::AddSlowCheckDestBuff;
    skill["AddSlowCheckSelfOwnBuff"]   = &Skill::AddSlowCheckSelfOwnBuff;
    skill["AddSlowCheckDestOwnBuff"]   = &Skill::AddSlowCheckDestOwnBuff;
    skill["AddCheckSelfLearntSkill"]   = &Skill::AddCheckSelfLearntSkill;
    skill["BindBuff"]                  = &Skill::BindBuff;
    skill["SetPublicCoolDown"]         = &Skill::SetPublicCoolDown;
    skill["SetNormalCoolDown"]         = &Skill::SetNormalCoolDown;
    skill["SetCheckCoolDown"]          = &Skill::SetCheckCoolDown;
    skill["dwLevelUpExp"]              = &Skill::dwLevelUpExp;
    skill["nExpAddOdds"]               = &Skill::nExpAddOdds;
    skill["nPlayerLevelLimit"]         = &Skill::nPlayerLevelLimit;
    skill["nBaseThreat"]               = &Skill::nBaseThreat;
    skill["nCostLife"]                 = &Skill::nCostLife;
    skill["nCostMana"]                 = &Skill::nCostMana;
    skill["nCostStamina"]              = &Skill::nCostStamina;
    skill["nCostItemType"]             = &Skill::nCostItemType;
    skill["nCostItemIndex"]            = &Skill::nCostItemIndex;
    skill["nCostSprintPower"]          = &Skill::nCostSprintPower;
    skill["bIsAccumulate"]             = &Skill::bIsAccumulate;
    skill["SetSubsectionSkill"]        = &Skill::SetSubsectionSkill;
    skill["nChainBranch"]              = &Skill::nChainBranch;
    skill["nChainDepth"]               = &Skill::nChainDepth;
    skill["nMinRadius"]                = &Skill::nMinRadius;
    skill["nMaxRadius"]                = &Skill::nMaxRadius;
    skill["nProtectRadius"]            = &Skill::nProtectRadius;
    skill["nHeight"]                   = &Skill::nHeight;
    skill["nRectWidth"]                = &Skill::nRectWidth;
    skill["nAngleRange"]               = &Skill::nAngleRange;
    skill["bFullAngleInAir"]           = &Skill::bFullAngleInAir;
    skill["nAreaRadius"]               = &Skill::nAreaRadius;
    skill["nTargetCountLimit"]         = &Skill::nTargetCountLimit;
    skill["bIgnorePrepareState"]       = &Skill::bIgnorePrepareState;
    skill["nPrepareFrames"]            = &Skill::nPrepareFrames;
    skill["nChannelFrame"]             = &Skill::nChannelFrame;
    skill["nBulletVelocity"]           = &Skill::nBulletVelocity;
    skill["bIsSunMoonPower"]           = &Skill::bIsSunMoonPower;
    skill["SetSunSubsectionSkill"]     = &Skill::SetSunSubsectionSkill;
    skill["SetMoonSubsectionSkill"]    = &Skill::SetMoonSubsectionSkill;
    skill["bIsFormationSkill"]         = &Skill::bIsFormationSkill;
    skill["nFormationRange"]           = &Skill::nFormationRange;
    skill["nLeastFormationPopulation"] = &Skill::nLeastFormationPopulation;
    skill["nTargetLifePercentMin"]     = &Skill::nTargetLifePercentMin;
    skill["nTargetLifePercentMax"]     = &Skill::nTargetLifePercentMax;
    skill["nSelfLifePercentMin"]       = &Skill::nSelfLifePercentMin;
    skill["nSelfLifePercentMax"]       = &Skill::nSelfLifePercentMax;
    skill["nBeatBackRate"]             = &Skill::nBeatBackRate;
    skill["nBrokenRate"]               = &Skill::nBrokenRate;
    skill["nBreakRate"]                = &Skill::nBreakRate;
    skill["nDismountingRate"]          = &Skill::nDismountingRate;
    skill["nWeaponDamagePercent"]      = &Skill::nWeaponDamagePercent;

    auto character = lua->new_usertype<Character>("Character");

    character["GetSelectCharacter"]     = &Character::characterGetSelect;
    character["GetSkillTarget"]         = &Character::characterGetTargetID;
    character["IsHaveBuff"]             = &Character::buffExist;
    character["GetBuff"]                = &Character::buffGet;
    character["GetBuffByOwner"]         = &Character::buffGetByOwner;
    character["AddBuff"]                = &Character::buffAddOptional;
    character["DelBuff"]                = &Character::buffDel;
    character["DelGroupBuff"]           = &Character::buffDelGroup;
    character["DelMultiGroupBuffByID"]  = &Character::buffDelMultiGroupByID;
    character["SetBuffLeftActiveCount"] = &Character::buffSetLeftActiveCount;
    character["SetBuffNextActiveFrame"] = &Character::buffSetNextActiveFrame;
    character["ClearCDTime"]            = &Character::cooldownClearTime;
    character["ModifyCoolDown"]         = &Character::cooldownModify;
    character["ResetCD"]                = &Character::cooldownReset;
    character["GetSkillLevel"]          = &Character::skillGetLevel;
    character["CastSkill"]              = sol::overload(
        static_cast<void (Character::*)(int, int)>(&Character::skillCast),
        static_cast<void (Character::*)(int, int, int)>(&Character::skillCast),
        static_cast<void (Character::*)(int, int, int, int)>(&Character::skillCast)
    );
    character["CastSkillXYZ"]        = &Character::skillCastXYZ;
    character["IsSkillRecipeActive"] = &Character::skillrecipeExist;
    character["GetScene"]            = &Character::sceneGet;
    character["SetTimer"]            = sol::overload(
        static_cast<void (Character::*)(int, std::string, int)>(&Character::timerSet),
        static_cast<void (Character::*)(int, std::string, int, int)>(&Character::timerSet)
    );
    character["IsFormationLeader"]         = &Character::otherIsFormationLeader;
    character["IsInParty"]                 = &Character::otherIsInParty;
    character["GetKungfuMountID"]          = &Character::otherGetKungfuMountID;
    character["GetMapID"]                  = &Character::otherGetMapID;
    character["ClearAllNpcThreat"]         = &Character::otherClearAllNpcThreat;
    character["CreatePublicShadow"]        = &Character::otherCreatePublicShadow;
    character["DestroyPublicShadow"]       = &Character::otherDestroyPublicShadow;
    character["DoAction"]                  = &Character::otherDoAction;
    character["PlayPublicShadowAnimation"] = &Character::otherPlayPublicShadowAnimation;
    character["dwID"]                      = &Character::dwID;
    character["nLevel"]                    = &Character::nLevel;
    character["nX"]                        = &Character::nX;
    character["nY"]                        = &Character::nY;
    character["nZ"]                        = &Character::nZ;
    character["nRoleType"]                 = &Character::nRoleType;
    character["nTouchRange"]               = &Character::nTouchRange;
    character["nCurrentSunEnergy"]         = &Character::nCurrentSunEnergy;
    character["nCurrentMoonEnergy"]        = &Character::nCurrentMoonEnergy;
    character["nSunPowerValue"]            = &Character::nSunPowerValue;
    character["nMoonPowerValue"]           = &Character::nMoonPowerValue;
    character["bSurplusAutoCast"]          = &Character::bSurplusAutoCast;
    character["bSurplusAutoReplenish"]     = &Character::bSurplusAutoReplenish;
    character["bFightState"]               = &Character::bFightState;
    character["fMaxLife64"]                = &Character::fMaxLife64;
    character["fCurrentLife64"]            = &Character::fCurrentLife64;

    auto buff                = lua->new_usertype<BuffItem>("Buff");
    buff["nLevel"]           = &BuffItem::nLevel;
    buff["nIndex"]           = &BuffItem::nIndex;
    buff["nStackNum"]        = &BuffItem::nStackNum;
    buff["nLeftActiveCount"] = &BuffItem::nLeftActiveCount;
    buff["nNextActiveFrame"] = &BuffItem::nNextActiveFrame;
    buff["nCustomValue"]     = &BuffItem::nCustomValue;

    auto scene     = lua->new_usertype<ChScene>("Scene");
    scene["nType"] = &ChScene::nType;

    // lua 函数

    (*lua)["Include"]                  = LuaGlobalFunction::Include;
    (*lua)["GetPlayer"]                = LuaGlobalFunction::GetPlayer;
    (*lua)["GetNpc"]                   = LuaGlobalFunction::GetNpc;
    (*lua)["IsPlayer"]                 = LuaGlobalFunction::IsPlayer;
    (*lua)["IsLangKeXingMap"]          = LuaGlobalFunction::IsLangKeXingMap;
    (*lua)["ModityCDToUI"]             = LuaGlobalFunction::ModityCDToUI;
    (*lua)["CheckInTongWar"]           = LuaGlobalFunction::CheckInTongWar;
    (*lua)["IsTreasureBattleFieldMap"] = LuaGlobalFunction::IsTreasureBattleFieldMap;
    (*lua)["GetValueByBits"]           = LuaGlobalFunction::GetValueByBits;
    (*lua)["SetValueByBits"]           = LuaGlobalFunction::SetValueByBits;
    (*lua)["RemoteCallToClient"]       = LuaGlobalFunction::RemoteCallToClient;
    (*lua)["GetDistanceSq"]            = LuaGlobalFunction::GetDistanceSq;
    (*lua)["Random"]                   = LuaGlobalFunction::Random;
    (*lua)["GetEditorString"]          = LuaGlobalFunction::GetEditorString;
    (*lua)["IsClient"]                 = LuaGlobalFunction::IsClient;

    // lua 常量

    (*lua)["CONSUME_BASE"]          = 100;
    (*lua)["LENGTH_BASE"]           = 64;
    (*lua)["MELEE_ATTACK_DISTANCE"] = 4 * 64;
    (*lua)["GLOBAL"]                = lua->create_table();
    (*lua)["GLOBAL"]["GAME_FPS"]    = 16;

    // lua 表

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

    sol::table ABSORB_ATTRIBUTE_SHIELD_TYPE = lua->create_table();
    for (int i = 0; i < static_cast<int>(ref::enumLuaAbsorbAttributeShieldType::COUNT); i++) {
        ROLE_TYPE[ref::refLuaAbsorbAttributeShieldType[i]] = i;
    }
    (*lua)["ABSORB_ATTRIBUTE_SHIELD_TYPE"] = ABSORB_ATTRIBUTE_SHIELD_TYPE;

    return lua;
}

void LuaGlobalFunction::Include(const std::string &filename) {
    LuaFunc::include(filename);
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

bool LuaGlobalFunction::IsLangKeXingMap(int mapID) {
    UNREFERENCED_PARAMETER(mapID);
    return false;
}

void LuaGlobalFunction::ModityCDToUI(frame::Character *character, int skillID, int c, int d) {
    UNREFERENCED_PARAMETER(character);
    UNREFERENCED_PARAMETER(skillID);
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(d);
    return;
}

bool LuaGlobalFunction::CheckInTongWar(frame::Character *character) {
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
        CONSTEXPR_LOG_ERROR(">>>>>>>CustomFunction.GetValueByBit Arg ERROR!!!!!BitIndex error{}", "");
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
        CONSTEXPR_LOG_ERROR(">>>>>>>CustomFunction.SetValueByBit Arg ERROR!!!!!nNewBit Must be 0 or 1,{}", "");
        return nValue;
    }
    if (nBit > 31 || nBit < 0) {
        CONSTEXPR_LOG_ERROR(">>>>>>>CustomFunction.SetValueByBit Arg ERROR!!!!!BitIndex error{}", "");
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

std::string LuaGlobalFunction::GetEditorString(int a, int b) {
    return std::to_string(a) + "-" + std::to_string(b);
}

bool LuaGlobalFunction::IsClient() {
    return false;
}
