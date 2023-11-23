#include "frame/global/skill.h"
#include "frame/lua_runtime.h"
#include "gdi.h"
#include "program/log.h"

using namespace ns_frame;

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
    std::lock_guard<std::mutex> lock(mutex); // 加锁
    // 可能有多个线程同时进入了 add 函数. 因此, 需要在加锁后再次判断技能是否存在.
    if (data.find(skillID) != data.end() && data[skillID].find(skillLevel) != data[skillID].end()) {
        // 若技能 ID 存在, 且技能等级存在, 则直接返回
        return; // 返回时, 会自动调用 lock 的析构函数, 从而释放锁
    }
    // 初始化技能
    Skill skill;
    skill.dwLevel = skillLevel;
    // 获取 tab
    if (data.find(skillID) == data.end()) {
        // 如果没有该技能 ID, 则先获取 tab
        gdi::TabSelectType arg;
        arg.emplace_back();
        arg[0]["SkillID"] = std::to_string(skillID);
        gdi::Interface::tabSelect(gdi::Tab::skills, arg);
        skill.tab = std::move(arg[0]);
    } else {
        // 如果该技能 ID 已存在, 则复用同 ID 技能的 tab
        auto it = data[skillID].begin();
        skill.tab = it->second.tab;
    }
    // 执行 GetSkillLevelData
    std::string name = "scripts\\skill\\" + skill.tab["ScriptFile"];
    bool res = LuaFunc::analysis(LuaFunc::getGetSkillLevelData(name)(skill), name, LuaFunc::Enum::GetSkillLevelData);
    if (res) {
        // 成功执行, 将技能添加到 data 中
        data[skillID][skillLevel] = std::move(skill);
    }
}

void Skill::AddAttribute_iiii(int a, int b, int c, int d) {
    attrAttributes.emplace_back(a, b, c, d);
}

void Skill::AddAttribute_iidi(int a, int b, double c, int d) {
    attrAttributes.emplace_back(a, b, c, d);
}

void Skill::AddAttribute_iisi(int a, int b, std::string c, int d) {
    attrAttributes.emplace_back(a, b, c, d);
}

void Skill::AddSlowCheckSelfBuff(int a, int b, int c, int d, int e) {
    attrCheckBuff.emplace_back(SkillCheckBuff::TypeEnum::self, a, b, c, d, e);
}

void Skill::AddSlowCheckDestBuff(int a, int b, int c, int d, int e) {
    attrCheckBuff.emplace_back(SkillCheckBuff::TypeEnum::dest, a, b, c, d, e);
}

void Skill::AddSlowCheckSelfOwnBuff(int a, int b, int c, int d, int e) {
    attrCheckBuff.emplace_back(SkillCheckBuff::TypeEnum::selfOwn, a, b, c, d, e);
}

void Skill::AddSlowCheckDestOwnBuff(int a, int b, int c, int d, int e) {
    attrCheckBuff.emplace_back(SkillCheckBuff::TypeEnum::destOwn, a, b, c, d, e);
}

void Skill::AddCheckSelfLearntSkill(int a, int b, int c) {
    attrCheckSelfLearntSkill.emplace_back(a, b, c);
}

void Skill::BindBuff(int a, int b, int c) {
    attrBindBuff.emplace_back(a, b, c);
}

void Skill::SetPublicCoolDown(int a) {
    attrCoolDown.emplace_back(SkillCoolDown::TypeEnum::publicCD, 0, a);
}

void Skill::SetNormalCoolDown(int a, int b) {
    attrCoolDown.emplace_back(SkillCoolDown::TypeEnum::normalCD, a, b);
}

void Skill::SetCheckCoolDown(int a, int b) {
    attrCoolDown.emplace_back(SkillCoolDown::TypeEnum::checkCD, a, b);
}

void Skill::SetSubsectionSkill(int a, int b, int c, int d) {
    LOG_ERROR("SetSubsectionSkill 未实现!\n");
}