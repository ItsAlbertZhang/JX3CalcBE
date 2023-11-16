#include "frame/skill.h"
#include "gdi.h"

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

    skill.dwLevel = skillLevel;
    res = skill.GetSkillLevelData(skill);
    if (!res.valid()) {
        sol::error err = res;
        std::cout << "GetSkillLevelData failed:\n"
                  << err.what() << std::endl;
    }
    // 将技能存入缓存
    data[skillID][skillLevel] = std::move(skill);
}

void Skill::AddAttribute_iiii(int a, int b, int c, int d) {
    attributes.emplace_back(a, b, c, d);
}

void Skill::AddAttribute_iidi(int a, int b, double c, int d) {
    AddAttribute_iiii(a, b, (int)c, d);
}

void Skill::AddAttribute_iisi(int a, int b, std::string c, int d) {
    attributes.emplace_back(-1, -1, -1, -1); // 占位
    attributesString.emplace_back(a, b, c, d);
}

void Skill::AddSlowCheckSelfBuff(int a, int b, int c, int d, int e) {
    std::cout << "AddSlowCheckSelfBuff"
              << "\t# " << a << "\t# " << b << "\t# " << c << "\t# " << d << "\t# " << e
              << std::endl;
}

void Skill::AddSlowCheckDestBuff(int a, int b, int c, int d, int e) {
    std::cout << "AddSlowCheckDestBuff"
              << "\t# " << a << "\t# " << b << "\t# " << c << "\t# " << d << "\t# " << e
              << std::endl;
}

void Skill::AddSlowCheckSelfOwnBuff(int a, int b, int c, int d, int e) {
    std::cout << "AddSlowCheckSelfOwnBuff"
              << "\t# " << a << "\t# " << b << "\t# " << c << "\t# " << d << "\t# " << e
              << std::endl;
}

void Skill::AddSlowCheckDestOwnBuff(int a, int b, int c, int d, int e) {
    std::cout << "AddSlowCheckDestOwnBuff"
              << "\t# " << a << "\t# " << b << "\t# " << c << "\t# " << d << "\t# " << e
              << std::endl;
}

void Skill::AddCheckSelfLearntSkill(int a, int b, int c) {
    std::cout << "AddCheckSelfLearntSkill"
              << "\t# " << a << "\t# " << b << "\t# " << c << std::endl;
}

void Skill::BindBuff(int a, int b, int c) {
    std::cout << "BindBuff"
              << "\t# " << a << "\t# " << b << "\t# " << c << std::endl;
}

void Skill::SetPublicCoolDown(int a) {
    std::cout << "SetPublicCoolDown"
              << "\t# " << a << std::endl;
}

void Skill::SetNormalCoolDown(int a, int b) {
    std::cout << "SetNormalCoolDown"
              << "\t# " << a << "\t# " << b << std::endl;
}

void Skill::SetCheckCoolDown(int a, int b) {
    std::cout << "SetCheckCoolDown"
              << "\t# " << a << "\t# " << b << std::endl;
}

void Skill::SetSubsectionSkill(int a, int b, int c, int d) {
    std::cerr << "SetSubsectionSkill 未实现!" << std::endl;
}