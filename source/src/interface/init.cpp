#include "interface/init.h"
#include "interface/global.h"
#include "interface/skill.h"

using namespace interface;

bool interface::luaPreprocess(sol::state &lua) {
    lua.new_usertype<InterfaceSkill>("InterfaceSkill",
                                     "AddAttribute", sol::overload(&InterfaceSkill::AddAttribute_iiii, &InterfaceSkill::AddAttribute_iisi, &InterfaceSkill::AddAttribute_iidi),
                                     "AddSlowCheckSelfBuff", &InterfaceSkill::AddSlowCheckSelfBuff,
                                     "BindBuff", &InterfaceSkill::BindBuff,
                                     "SetPublicCoolDown", &InterfaceSkill::SetPublicCoolDown,
                                     "SetNormalCoolDown", &InterfaceSkill::SetNormalCoolDown,
                                     "SetCheckCoolDown", &InterfaceSkill::SetCheckCoolDown,
                                     "dwLevel", &InterfaceSkill::dwLevel,
                                     "nCostSprintPower", &InterfaceSkill::nCostSprintPower,
                                     "nMinRadius", &InterfaceSkill::nMinRadius,
                                     "nMaxRadius", &InterfaceSkill::nMaxRadius,
                                     "nAngleRange", &InterfaceSkill::nAngleRange,
                                     "bFullAngleInAir", &InterfaceSkill::bFullAngleInAir,
                                     "nChannelInterval", &InterfaceSkill::nChannelInterval,
                                     "nWeaponDamagePercent", &InterfaceSkill::nWeaponDamagePercent);
    lua.set_function("Include", GlobalFunction::Include);

    sol::table AttributeEffectMode = lua.create_table();
    for (int i = 0; i < static_cast<int>(GlobalEnum::ATTRIBUTE_EFFECT_MODE::COUNT); i++) {
        AttributeEffectMode[GlobalEnum::AttributeEffectMode[i]] = i;
    }
    lua["ATTRIBUTE_EFFECT_MODE"] = AttributeEffectMode;
    sol::table AttributeType = lua.create_table();
    for (int i = 0; i < static_cast<int>(GlobalEnum::ATTRIBUTE_TYPE::COUNT); i++) {
        AttributeType[GlobalEnum::AttributeType[i]] = i;
    }
    lua["ATTRIBUTE_TYPE"] = AttributeType;
    sol::table BuffCompareFlag = lua.create_table();
    for (int i = 0; i < static_cast<int>(GlobalEnum::BUFF_COMPARE_FLAG::COUNT); i++) {
        BuffCompareFlag[GlobalEnum::BuffCompareFlag[i]] = i;
    }
    lua["BUFF_COMPARE_FLAG"] = BuffCompareFlag;

    lua["CONSUME_BASE"] = 100;
    lua["LENGTH_BASE"] = 64;

    return true;
}

bool (*interface::Init::init())(sol::state &lua) {
    return interface::luaPreprocess;
}