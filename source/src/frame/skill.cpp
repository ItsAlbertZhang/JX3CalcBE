#include "frame/skill.h"
#include "gdi.h"
#include "interface/skill.h"

using namespace ns_frame;

// 静态成员变量初始化
std::unordered_map<int, std::unordered_map<int, Skill>> SkillManager::data;

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
        if (res.valid()) {
            skill.GetSkillLevelData = gdi::ptrInterface->luaGetFunction("GetSkillLevelData");
            // } else {
            //     sol::error err = res;
            //     std::cout << "luaExecuteFile failed:\n"
            //               << err.what() << std::endl;
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
        // } else {
        //     std::cout << "skill.nCostSprintPower = " << interfaceskill.nCostSprintPower << std::endl;
        //     std::cout << "skill.nMinRadius = " << interfaceskill.nMinRadius << std::endl;
        //     std::cout << "skill.nMaxRadius = " << interfaceskill.nMaxRadius << std::endl;
        //     std::cout << "skill.nAngleRange = " << interfaceskill.nAngleRange << std::endl;
        //     std::cout << "skill.bFullAngleInAir = " << interfaceskill.bFullAngleInAir << std::endl;
        //     std::cout << "skill.nChannelInterval = " << interfaceskill.nChannelInterval << std::endl;
        //     std::cout << "skill.nWeaponDamagePercent = " << interfaceskill.nWeaponDamagePercent << std::endl;
    }
}

void Skill::cast() {
    std::cout << "Skill::cast()" << std::endl;
}