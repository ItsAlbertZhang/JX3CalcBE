#include "frame/lua.h"
#include "frame/interface.h"
#include "frame/skill.h"

using namespace ns_frame;

bool ns_frame::lua_init(sol::state &lua) {
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

                            "nPrepareFrames", &Skill::nPrepareFrames,
                            "nChannelFrame", &Skill::nChannelFrame,
                            "nBulletVelocity", &Skill::nBulletVelocity,

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
    lua.set_function("Include", LuaGlobalFunction::Include);

    sol::table AttributeEffectMode = lua.create_table();
    for (int i = 0; i < static_cast<int>(LuaGlobalTable::ATTRIBUTE_EFFECT_MODE::COUNT); i++) {
        AttributeEffectMode[ns_interface::luaAttributeEffectMode[i]] = i;
    }
    lua["ATTRIBUTE_EFFECT_MODE"] = AttributeEffectMode;
    sol::table AttributeType = lua.create_table();
    for (int i = 0; i < static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::COUNT); i++) {
        AttributeType[ns_interface::luaAttributeType[i]] = i;
    }
    lua["ATTRIBUTE_TYPE"] = AttributeType;
    sol::table BuffCompareFlag = lua.create_table();
    for (int i = 0; i < static_cast<int>(LuaGlobalTable::BUFF_COMPARE_FLAG::COUNT); i++) {
        BuffCompareFlag[ns_interface::luaBuffCompareFlag[i]] = i;
    }
    lua["BUFF_COMPARE_FLAG"] = BuffCompareFlag;

    lua["CONSUME_BASE"] = 100;
    lua["LENGTH_BASE"] = 64;

    return true;
}

void LuaGlobalFunction::Include(const std::string &filename) {
    return;
}
