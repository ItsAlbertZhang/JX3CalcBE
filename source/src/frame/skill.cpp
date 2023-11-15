#include "frame/skill.h"
#include "frame/character.h"
#include "gdi.h"
#include "interface/lua.h"
#include "interface/lua_skill.h"

using namespace ns_frame;

// 静态成员变量初始化
thread_local std::unordered_map<int, std::unordered_map<int, Skill>> SkillManager::data;

Skill &SkillManager::get(int skillID, int skillLevel) {
    // 若技能 ID 不存在, 则添加
    if (data.find(skillID) == data.end()) {
        add(skillID, skillLevel);
    } else {
        // 若技能 ID 存在, 但技能等级不存在, 则添加
        if (data[skillID].find(skillLevel) == data[skillID].end()) {
            add(skillID, skillLevel);
        }
    }
    return data[skillID][skillLevel];
}

void SkillManager::add(int skillID, int skillLevel) {
    // 初始化技能
    Skill skill;
    sol::protected_function_result res;
    // 获取 tab 和 GetSkillLevelData 函数. 这里省略了非常多的错误检查.
    if (data.find(skillID) == data.end()) {
        // 获取 tab
        gdi::TabSelectType arg;
        arg.emplace_back();
        arg[0]["SkillID"] = std::to_string(skillID);
        gdi::ptrInterface->tabSelect(gdi::Tab::skills, arg);
        skill.tab = std::move(arg[0]);
        std::cout << skill.tab["ScriptFile"] << std::endl;
        // 获取 GetSkillLevelData 函数
        res = gdi::ptrInterface->luaExecuteFile("scripts\\skill\\" + skill.tab["ScriptFile"]);
        if (!res.valid()) {
            sol::error err = res;
            std::cout << "luaExecuteFile failed:\n"
                      << err.what() << std::endl;
        } else {
            skill.GetSkillLevelData = gdi::ptrInterface->luaGetFunction("GetSkillLevelData");
        }
    } else {
        auto it = data[skillID].begin();
        skill.tab = it->second.tab;
        skill.GetSkillLevelData = it->second.GetSkillLevelData;
    }
    // 调用 GetSkillLevelData 函数, 初始化技能
    ns_interface::InterfaceSkill interfaceskill;
    ns_interface::InterfaceSkill::current_skill = &skill;
    interfaceskill.dwLevel = skillLevel;
    res = skill.GetSkillLevelData(interfaceskill);
    if (!res.valid()) {
        sol::error err = res;
        std::cout << "GetSkillLevelData failed:\n"
                  << err.what() << std::endl;
    }
    // 将技能存入缓存
    data[skillID][skillLevel] = std::move(skill);
}

void Character::LearnSkill(int skillID, int skillLevel) {
    skillLearned[skillID] = skillLevel;
    SkillManager::get(skillID, skillLevel);
}

void Character::CastSkill(int skillID, int skillLevel) {
    // 获取技能
    Skill &skill = SkillManager::get(skillID, skillLevel);
    // test, 遍历 Attribute
    int current_attribute_string_index = 0;
    for (auto &it : skill.attributes) {
        if (it.type >= 0) {
            std::cout << ns_interface::LuaGlobalTable::AttributeEffectMode[it.mode] << "\n"
                      << ns_interface::LuaGlobalTable::AttributeType[it.type] << "\n"
                      << it.param1 << "\t" << it.param2 << "\n"
                      << std::endl;
        } else {
            auto &it_string = skill.attributesString[current_attribute_string_index++];
            std::cout << ns_interface::LuaGlobalTable::AttributeEffectMode[it_string.mode] << "\n"
                      << ns_interface::LuaGlobalTable::AttributeType[it_string.type] << "\n"
                      << it_string.param1 << "\t" << it_string.param2 << "\n"
                      << std::endl;
        }
    }

    // switch (type) {
    // case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::CAST_SKILL_TARGET_DST):
    //     std::cout << "CAST_SKILL_TARGET_DST" << std::endl;
    // default:
    //     std::cerr << "位置类型:\t" << type << "\t" << LuaGlobalTable::AttributeType[type] << std::endl;
    // }
}