#include "frame/character.h"
#include "frame/base_skill.h"
#include "frame/lua_func.h"
#include "frame/lua_interface.h"
#include "program/log.h"

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
    LOG_INFO("\nTry to CastSkill: %d # %d\n", skillID, skillLevel);
    // 获取技能
    Skill &skill = SkillManager::get(skillID, skillLevel);

    // 检查技能是否可以释放
    ns_frame::CharacterBuff *checkbuffObj = nullptr;
    Character *checkbuffCharacter = nullptr;
    bool checkbuffSrcOwn = false;
    for (auto &it : skill.checkBuff) {
        if (it.nStackNum == 0 || it.nLevel == 0) {
            /**
             * @brief 被废弃但未被从 lua 代码中移除的检查
             * @note 出现在 明教_烈日斩.lua 中, 检查 6279, 为贪魔体 buff
             * @author ItsAlbertZhang
             */
            continue;
        }
        switch (it.type) {
        case Skill::CheckBuffEnum::self:
            checkbuffCharacter = this;
            break;
        case Skill::CheckBuffEnum::dest:
            checkbuffCharacter = this->target;
            break;
        case Skill::CheckBuffEnum::selfOwn:
            checkbuffCharacter = this;
            checkbuffSrcOwn = true;
            break;
        case Skill::CheckBuffEnum::destOwn:
            checkbuffCharacter = this->target;
            checkbuffSrcOwn = true;
            break;
        }
        if (!checkbuffCharacter->hasBuff(it.dwBuffID, it.nLevel)) {
            return; // 找不到符合要求的 buff, CastSkill 失败
        }
        checkbuffObj = &(checkbuffCharacter->buffExist[it.dwBuffID][it.nLevel]);
        if (checkbuffSrcOwn && checkbuffObj->source != this) {
            return; // buff 来源不是自己, CastSkill 失败
        }
        if (!Character::luaBuffCompare(it.nStackNumCompareFlag, it.nStackNum, checkbuffObj->nStackNum)) {
            return; // buff 比较不符合要求, CastSkill 失败}
        }
    }

    LOG_INFO("%d # %d cast successfully!\n", skillID, skillLevel);
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
                LOG_INFO("EXECUTE_SCRIPT: %s # %d\n", currStrAttrParamStr.c_str(), currStrAttr->param2);
                break;
            case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::EXECUTE_SCRIPT_WITH_PARAM):
                currStrAttrParamStr = "scripts/" + currStrAttr->param1;
                luaFunc = LuaApply::get(currStrAttrParamStr);
                LOG_INFO("EXECUTE_SCRIPT_WITH_PARAM: %s # %d\n", currStrAttrParamStr, currStrAttr->param2);
                break;
            default:
                std::cerr << "Undefined type: " << currStrAttr->type << "\t"
                          << LuaTableString::luaAttributeType[currStrAttr->type] << std::endl;
            }
            currStrAttrIdx++;
            break;
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::CAST_SKILL_TARGET_DST):
            skillQueue.emplace(it.param1, it.param2);
            LOG_INFO("CAST_SKILL_TARGET_DST: %d # %d\n", it.param1, it.param2);
            break;
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::CAST_SKILL):
            skillQueue.emplace(it.param1, it.param2);
            LOG_INFO("CAST_SKILL: %d # %d\n", it.param1, it.param2);
            break;
        default:
            std::cerr << "Undefined type: " << it.type << "\t" << LuaTableString::luaAttributeType[it.type] << std::endl;
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

bool Character::hasBuff(int buffID, int buffLevel) {
    return this->buffExist.find(buffID) != this->buffExist.end() && this->buffExist[buffID].find(buffLevel) != this->buffExist[buffID].end();
}

bool Character::luaBuffCompare(int flag, int luaValue, int buffValue) {
    switch (flag) {
    case static_cast<int>(LuaGlobalTable::BUFF_COMPARE_FLAG::EQUAL):
        return buffValue == luaValue;
        break;
    case static_cast<int>(LuaGlobalTable::BUFF_COMPARE_FLAG::GREATER):
        return buffValue > luaValue;
        break;
    case static_cast<int>(LuaGlobalTable::BUFF_COMPARE_FLAG::GREATER_EQUAL):
        return buffValue >= luaValue;
        break;
    }
    return false;
}