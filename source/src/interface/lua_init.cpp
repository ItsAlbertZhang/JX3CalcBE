#include "interface/lua.h"
#include "interface/skill.h"

using namespace ns_interface;

bool ns_interface::lua_init(sol::state &lua) {
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