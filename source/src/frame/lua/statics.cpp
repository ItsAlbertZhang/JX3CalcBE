#include "frame/lua/statics.h"
#include "frame/character/character.h"
#include "frame/global/skill.h"
#include "frame/lua/global_func.h"
#include "frame/lua/interface.h"
#include "frame/ref/lua_attribute_type.h"
#include "frame/ref/lua_normal.h"
#include "frame/ref/lua_special.h"
#include <optional>

using namespace jx3calc;
using namespace frame;

const std::unordered_set<std::string> frame::lua::statics::LuaBlacklistFiles = {
    "scripts/player/include/Kungfu2ArenaType.lh",
};

const char *const frame::lua::statics::luaFuncList[] {
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
    "LearnSkillLevel",
    "SuperCustomDamage",
};
const size_t frame::lua::statics::luaFuncListSize =
    sizeof(frame::lua::statics::luaFuncList) / sizeof(frame::lua::statics::luaFuncList[0]);

static void FileNotExistEmptyFunction() {
    // do nothing
}

sol::state *frame::lua::statics::luaInit() {
    auto lua = lua::interface::getLuaState();
    lua->open_libraries(sol::lib::base, sol::lib::table, sol::lib::math);

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
    skill["nCostManaBasePercent"]      = &Skill::nCostManaBasePercent;
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
    character["ModifyCoolDown"]         = static_cast<void (Character::*)(int, int)>(&Character::cooldownModify);
    character["ResetCD"]                = &Character::cooldownReset;
    character["GetSkillLevel"]          = &Character::skillGetLevel;
    character["LearnSkillLevel"]        = &Character::skillLearn;
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
    character["SuperCustomDamage"]         = &Character::otherSuperCustomDamage;
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

    (*lua)["FileNotExistEmptyFunction"] = &FileNotExistEmptyFunction;

    (*lua)["Include"]                  = &lua::gfunc::Include;
    (*lua)["GetPlayer"]                = &lua::gfunc::GetPlayer;
    (*lua)["GetNpc"]                   = &lua::gfunc::GetNpc;
    (*lua)["IsPlayer"]                 = &lua::gfunc::IsPlayer;
    (*lua)["IsLangKeXingMap"]          = &lua::gfunc::IsLangKeXingMap;
    (*lua)["ModityCDToUI"]             = &lua::gfunc::ModityCDToUI;
    (*lua)["CheckInTongWar"]           = &lua::gfunc::CheckInTongWar;
    (*lua)["IsTreasureBattleFieldMap"] = &lua::gfunc::IsTreasureBattleFieldMap;
    (*lua)["GetValueByBits"]           = &lua::gfunc::GetValueByBits;
    (*lua)["SetValueByBits"]           = &lua::gfunc::SetValueByBits;
    (*lua)["RemoteCallToClient"]       = &lua::gfunc::RemoteCallToClient;
    (*lua)["GetDistanceSq"]            = &lua::gfunc::GetDistanceSq;
    (*lua)["Random"]                   = &lua::gfunc::Random;
    (*lua)["GetEditorString"]          = &lua::gfunc::GetEditorString;
    (*lua)["IsClient"]                 = &lua::gfunc::IsClient;

    // lua 常量

    (*lua)["CONSUME_BASE"]          = 100;
    (*lua)["LENGTH_BASE"]           = 64;
    (*lua)["MELEE_ATTACK_DISTANCE"] = 4 * 64;
    (*lua)["GLOBAL"]                = lua->create_table();
    (*lua)["GLOBAL"]["GAME_FPS"]    = 16;

    // lua 表

    sol::table AttributeType = lua->create_table();
    for (int i = 0; i < static_cast<int>(Ref<ref::lua::ATTRIBUTE_TYPE>::count); i++) {
        AttributeType[Ref<ref::lua::ATTRIBUTE_TYPE>::names[i]] = i;
    }
    (*lua)["ATTRIBUTE_TYPE"] = AttributeType;

    sol::table AttributeEffectMode = lua->create_table();
    for (int i = 0; i < static_cast<int>(Ref<ref::lua::ATTRIBUTE_EFFECT_MODE>::count); i++) {
        AttributeEffectMode[Ref<ref::lua::ATTRIBUTE_EFFECT_MODE>::names[i]] = i;
    }
    (*lua)["ATTRIBUTE_EFFECT_MODE"] = AttributeEffectMode;

    sol::table BuffCompareFlag = lua->create_table();
    for (int i = 0; i < static_cast<int>(Ref<ref::lua::BUFF_COMPARE_FLAG>::count); i++) {
        BuffCompareFlag[Ref<ref::lua::BUFF_COMPARE_FLAG>::names[i]] = i;
    }
    (*lua)["BUFF_COMPARE_FLAG"] = BuffCompareFlag;

    sol::table SKILL_COMPARE_FLAG = lua->create_table();
    for (int i = 0; i < static_cast<int>(Ref<ref::lua::BUFF_COMPARE_FLAG>::count); i++) {
        SKILL_COMPARE_FLAG[Ref<ref::lua::BUFF_COMPARE_FLAG>::names[i]] = i;
    }
    (*lua)["SKILL_COMPARE_FLAG"] = SKILL_COMPARE_FLAG;

    sol::table TARGET = lua->create_table();
    for (int i = 0; i < static_cast<int>(Ref<ref::lua::TARGET>::count); i++) {
        TARGET[Ref<ref::lua::TARGET>::names[i]] = i;
    }
    (*lua)["TARGET"] = TARGET;

    sol::table SKILL_KIND_TYPE = lua->create_table();
    for (int i = 0; i < static_cast<int>(Ref<ref::lua::SKILL_KIND_TYPE>::count); i++) {
        SKILL_KIND_TYPE[Ref<ref::lua::SKILL_KIND_TYPE>::names[i]] = i;
    }
    (*lua)["SKILL_KIND_TYPE"] = SKILL_KIND_TYPE;

    sol::table ROLE_TYPE = lua->create_table();
    for (int i = 0; i < static_cast<int>(Ref<ref::lua::ROLE_TYPE>::count); i++) {
        ROLE_TYPE[Ref<ref::lua::ROLE_TYPE>::names[i]] = i;
    }
    (*lua)["ROLE_TYPE"] = ROLE_TYPE;

    sol::table ABSORB_ATTRIBUTE_SHIELD_TYPE = lua->create_table();
    for (int i = 0; i < static_cast<int>(Ref<ref::lua::ABSORB_ATTRIBUTE_SHIELD_TYPE>::count); i++) {
        ABSORB_ATTRIBUTE_SHIELD_TYPE[Ref<ref::lua::ABSORB_ATTRIBUTE_SHIELD_TYPE>::names[i]] = i;
    }
    (*lua)["ABSORB_ATTRIBUTE_SHIELD_TYPE"] = ABSORB_ATTRIBUTE_SHIELD_TYPE;

    sol::table MOVE_STATE = lua->create_table();
    for (int i = 0; i < static_cast<int>(Ref<ref::lua::MOVE_STATE>::count); i++) {
        MOVE_STATE[Ref<ref::lua::MOVE_STATE>::names[i]] = i;
    }
    (*lua)["MOVE_STATE"] = MOVE_STATE;

    sol::table KUNGFU_ADAPTIVETYPE_LIST = lua->create_table();
    for (auto &it : Ref<ref::lua::KUNGFU_ADAPTIVETYPE_LIST>::kungfu2type) {
        sol::table sub_table                                 = lua->create_table();
        sub_table["adaptiveType"]                            = static_cast<int>(it.second);
        KUNGFU_ADAPTIVETYPE_LIST[static_cast<int>(it.first)] = sub_table;
    }
    (*lua)["KUNGFU_ADAPTIVETYPE_LIST"] = KUNGFU_ADAPTIVETYPE_LIST;

    return lua;
}
