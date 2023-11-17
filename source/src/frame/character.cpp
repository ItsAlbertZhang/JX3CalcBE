#include "frame/character.h"
#include "frame/interface.h"
#include "frame/lua.h"
#include "frame/lua_func.h"
#include "frame/skill.h"

using namespace ns_frame;

std::vector<Character *> Character::characterList;

Character::Character() {
    nCharacterID = static_cast<int>(characterList.size());
    characterList.push_back(this);
}

void Character::LearnSkill(int skillID, int skillLevel) {
    skillLearned[skillID] = skillLevel;
    SkillManager::get(skillID, skillLevel);
}

void Character::CastSkill(int skillID, int skillLevel) {
    std::cout << "\nCastSkill: " << skillID << " # " << skillLevel << std::endl;
    // 获取技能
    Skill &skill = SkillManager::get(skillID, skillLevel);
    // 检查技能是否可以释放
    for (auto &it : skill.checkBuff) {
        switch (it.type) {
        }
    }
    // 提前准备 switch 语句需要的资源
    int currStrAttrIdx = 0;                             // AddAttribute 添加的参数1为字符串的属性列表的当前下标
    Skill::SkillAttributeString *currStrAttr = nullptr; // AddAttribute 添加的参数1为字符串的属性列表的当前元素
    std::string currStrAttrParamStr;                    // 当前元素的 param1
    sol::protected_function luaFunc;                    // 用于提取 Lua 脚本中的函数
    // 根据 type 执行对应的操作
    for (auto &it : skill.attributes) {
        switch (it.type) {
        case -1:
            currStrAttr = &(skill.attributesString[currStrAttrIdx]);
            switch (currStrAttr->type) {
            case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::EXECUTE_SCRIPT):
                currStrAttrParamStr = "scripts/" + currStrAttr->param1;
                luaFunc = LuaApply::get(currStrAttrParamStr);
                std::cout << "EXECUTE_SCRIPT: " << currStrAttrParamStr << " # " << currStrAttr->param2 << std::endl;
                break;
            case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::EXECUTE_SCRIPT_WITH_PARAM):
                currStrAttrParamStr = "scripts/" + currStrAttr->param1;
                luaFunc = LuaApply::get(currStrAttrParamStr);
                std::cout << "EXECUTE_SCRIPT_WITH_PARAM: " << currStrAttrParamStr << " # " << currStrAttr->param2 << std::endl;
                break;
            default:
                std::cerr << "Undefined type: " << currStrAttr->type << "\t"
                          << ns_interface::luaAttributeType[currStrAttr->type] << std::endl;
            }
            currStrAttrIdx++;
            break;
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::CAST_SKILL_TARGET_DST):
            skillQueue.push({.skillID = it.param1, .skillLevel = it.param2});
            std::cout << "CAST_SKILL_TARGET_DST: " << it.param1 << " # " << it.param2 << std::endl;
            break;
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::CAST_SKILL):
            skillQueue.push({.skillID = it.param1, .skillLevel = it.param2});
            std::cout << "CAST_SKILL: " << it.param1 << " # " << it.param2 << std::endl;
            break;
        default:
            std::cerr << "Undefined type: " << it.type << "\t" << ns_interface::luaAttributeType[it.type] << std::endl;
        }
    }
    while (!skillQueue.empty()) {
        auto it = skillQueue.front();
        skillQueue.pop();
        this->CastSkill(it.skillID, it.skillLevel);
    }
}

void Character::AddBuff(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel) {
}