#include "interface/lua.h"
#include "interface/lua_skill.h"

using namespace ns_interface;

bool ns_interface::lua_init(sol::state &lua) {
    lua.new_usertype<InterfaceSkill>("InterfaceSkill",
                                     "dwLevel", &InterfaceSkill::dwLevel,
                                     "nChannelInterval", &InterfaceSkill::nChannelInterval,

                                     "AddAttribute", sol::overload(&InterfaceSkill::AddAttribute_iiii, &InterfaceSkill::AddAttribute_iisi, &InterfaceSkill::AddAttribute_iidi),

                                     "AddSlowCheckSelfBuff", &InterfaceSkill::AddSlowCheckSelfBuff,
                                     "AddSlowCheckDestBuff", &InterfaceSkill::AddSlowCheckDestBuff,
                                     "AddSlowCheckSelfOwnBuff", &InterfaceSkill::AddSlowCheckSelfOwnBuff,
                                     "AddSlowCheckDestOwnBuff", &InterfaceSkill::AddSlowCheckDestOwnBuff,

                                     "AddCheckSelfLearntSkill", &InterfaceSkill::AddCheckSelfLearntSkill,

                                     "BindBuff", &InterfaceSkill::BindBuff,

                                     "SetPublicCoolDown", &InterfaceSkill::SetPublicCoolDown,
                                     "SetNormalCoolDown", &InterfaceSkill::SetNormalCoolDown,
                                     "SetCheckCoolDown", &InterfaceSkill::SetCheckCoolDown,

                                     "dwLevelUpExp", &InterfaceSkill::dwLevelUpExp,
                                     "nExpAddOdds", &InterfaceSkill::nExpAddOdds,
                                     "nPlayerLevelLimit", &InterfaceSkill::nPlayerLevelLimit,

                                     "nBaseThreat", &InterfaceSkill::nBaseThreat,

                                     "nCostLife", &InterfaceSkill::nCostLife,
                                     "nCostMana", &InterfaceSkill::nCostMana,
                                     "nCostStamina", &InterfaceSkill::nCostStamina,
                                     "nCostItemType", &InterfaceSkill::nCostItemType,
                                     "nCostItemIndex", &InterfaceSkill::nCostItemIndex,
                                     "nCostSprintPower", &InterfaceSkill::nCostSprintPower,

                                     "bIsAccumulate", &InterfaceSkill::bIsAccumulate,

                                     "SetSubsectionSkill", &InterfaceSkill::SetSubsectionSkill,
                                     "nChainBranch", &InterfaceSkill::nChainBranch,
                                     "nChainDepth", &InterfaceSkill::nChainDepth,

                                     "nMinRadius", &InterfaceSkill::nMinRadius,
                                     "nMaxRadius", &InterfaceSkill::nMaxRadius,

                                     "nProtectRadius", &InterfaceSkill::nProtectRadius,
                                     "nHeight", &InterfaceSkill::nHeight,
                                     "nRectWidth", &InterfaceSkill::nRectWidth,
                                     "nAngleRange", &InterfaceSkill::nAngleRange,
                                     "bFullAngleInAir", &InterfaceSkill::bFullAngleInAir,
                                     "nAreaRadius", &InterfaceSkill::nAreaRadius,
                                     "nTargetCountLimit", &InterfaceSkill::nTargetCountLimit,

                                     "nPrepareFrames", &InterfaceSkill::nPrepareFrames,
                                     "nChannelFrame", &InterfaceSkill::nChannelFrame,
                                     "nBulletVelocity", &InterfaceSkill::nBulletVelocity,

                                     "bIsFormationSkill", &InterfaceSkill::bIsFormationSkill,
                                     "nFormationRange", &InterfaceSkill::nFormationRange,
                                     "nLeastFormationPopulation", &InterfaceSkill::nLeastFormationPopulation,

                                     "nTargetLifePercentMin", &InterfaceSkill::nTargetLifePercentMin,
                                     "nTargetLifePercentMax", &InterfaceSkill::nTargetLifePercentMax,

                                     "nSelfLifePercentMin", &InterfaceSkill::nSelfLifePercentMin,
                                     "nSelfLifePercentMax", &InterfaceSkill::nSelfLifePercentMax,

                                     "nBeatBackRate", &InterfaceSkill::nBeatBackRate,
                                     "nBrokenRate", &InterfaceSkill::nBrokenRate,
                                     "nBreakRate", &InterfaceSkill::nBreakRate,
                                     "nDismountingRate", &InterfaceSkill::nDismountingRate,

                                     "nWeaponDamagePercent", &InterfaceSkill::nWeaponDamagePercent);
    lua.set_function("Include", LuaGlobalFunction::Include);

    sol::table AttributeEffectMode = lua.create_table();
    for (int i = 0; i < static_cast<int>(LuaGlobalTable::ATTRIBUTE_EFFECT_MODE::COUNT); i++) {
        AttributeEffectMode[LuaGlobalTable::AttributeEffectMode[i]] = i;
    }
    lua["ATTRIBUTE_EFFECT_MODE"] = AttributeEffectMode;
    sol::table AttributeType = lua.create_table();
    for (int i = 0; i < static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::COUNT); i++) {
        AttributeType[LuaGlobalTable::AttributeType[i]] = i;
    }
    lua["ATTRIBUTE_TYPE"] = AttributeType;
    sol::table BuffCompareFlag = lua.create_table();
    for (int i = 0; i < static_cast<int>(LuaGlobalTable::BUFF_COMPARE_FLAG::COUNT); i++) {
        BuffCompareFlag[LuaGlobalTable::BuffCompareFlag[i]] = i;
    }
    lua["BUFF_COMPARE_FLAG"] = BuffCompareFlag;

    lua["CONSUME_BASE"] = 100;
    lua["LENGTH_BASE"] = 64;

    return true;
}