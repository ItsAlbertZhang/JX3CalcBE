#include "frame/character/character.h"
#include "frame/global/skill.h"
#include "frame/lua_runtime.h"
#include "frame/lua_static.h"
#include "program/log.h"

using namespace ns_frame;

static bool staticCheckBuff(Character *self, const Skill &skill);
static inline bool staticCheckBuffCompare(int flag, int luaValue, int buffValue);
static bool staticCheckSelfLearntSkill(Character *self, const Skill &skill);
static inline bool staticCheckSelfLearntSkillCompare(int flag, int luaValue, int skillValue);
static bool staticCheckCoolDown(Character *self, const Skill &skill);

void Character::CastSkill(int skillID, int skillLevel) {
    LOG_INFO("\nTry to CastSkill: %d # %d\n", skillID, skillLevel);
    // 获取技能
    Skill &skill = SkillManager::get(skillID, skillLevel);

    // 检查技能是否可以释放
    if (!staticCheckBuff(this, skill)) {
        LOG_INFO("Checkbuff failed!\n");
        return;
    }
    if (!staticCheckSelfLearntSkill(this, skill)) {
        LOG_INFO("SkillCheckSelfLearntSkill failed!\n");
        return;
    }

    LOG_INFO("%d # %d cast successfully!\n", skillID, skillLevel);
    // 提前准备 switch 语句需要的资源
    std::string currStrAttrParamStr; // 当前元素的 param1
    sol::protected_function luaFunc; // 用于提取 Lua 脚本中的函数
    // 根据 type 执行对应的操作
    for (auto &it : skill.attrAttributes) {
        switch (it.type) {
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::EXECUTE_SCRIPT):
            currStrAttrParamStr = "scripts/" + it.param1Str;
            luaFunc = LuaFunc::getApply(currStrAttrParamStr);
            LOG_INFO("EXECUTE_SCRIPT: %s # %d\n", currStrAttrParamStr.c_str(), it.param2);
            break;
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::EXECUTE_SCRIPT_WITH_PARAM):
            currStrAttrParamStr = "scripts/" + it.param1Str;
            luaFunc = LuaFunc::getApply(currStrAttrParamStr);
            LOG_INFO("EXECUTE_SCRIPT_WITH_PARAM: %s # %d\n", currStrAttrParamStr, it.param2);
            break;
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::CAST_SKILL_TARGET_DST):
            this->chSkill.skillQueue.emplace(it.param1Int, it.param2);
            LOG_INFO("CAST_SKILL_TARGET_DST: %d # %d\n", it.param1Int, it.param2);
            break;
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::CAST_SKILL):
            this->chSkill.skillQueue.emplace(it.param1Int, it.param2);
            LOG_INFO("CAST_SKILL: %d # %d\n", it.param1Int, it.param2);
            break;
        default:
            LOG_ERROR("Undefined type: %d\t%s\n", it.type, LuaTableString::luaAttributeType[it.type]);
        }
    }
    while (!this->chSkill.skillQueue.empty()) {
        auto it = this->chSkill.skillQueue.front();
        this->chSkill.skillQueue.pop();
        this->CastSkill(it.skillID, it.skillLevel);
    }
}

static bool staticCheckBuff(Character *self, const Skill &skill) {
    for (const auto &it : skill.attrCheckBuff) {
        Character *checkbuffCharacter = nullptr;
        bool checkbuffSrcOwn = false;
        if (it.nStackNum == 0 || it.nLevel == 0) {
            /**
             * @note 出现在 明教_烈日斩.lua 等文件中, 检查 6279, 为贪魔体 buff
             * @warning 暂时未知: 检查 "层数为 0, Level 为 0" 是何种逻辑
             * @warning - 可能逻辑 1: **不检查** 该 buff.
             * @warning - 可能逻辑 2: 要求 **不存在** 该 buff.
             * @warning 此处按照逻辑 1 处理. 若日后发现实际使用逻辑 2, 则需要修改此处.
             * @author ItsAlbertZhang
             */
            continue;
        }
        switch (it.type) {
        case Skill::SkillCheckBuff::TypeEnum::self:
            checkbuffCharacter = self;
            break;
        case Skill::SkillCheckBuff::TypeEnum::dest:
            checkbuffCharacter = self->target;
            break;
        case Skill::SkillCheckBuff::TypeEnum::selfOwn:
            checkbuffCharacter = self;
            checkbuffSrcOwn = true;
            break;
        case Skill::SkillCheckBuff::TypeEnum::destOwn:
            checkbuffCharacter = self->target;
            checkbuffSrcOwn = true;
            break;
        }
        if (!checkbuffCharacter->hasBuff(it.dwBuffID, it.nLevel)) {
            return false; // 找不到符合要求的 buff, CastSkill 失败
        }
        ns_frame::CharacterBuff::Item *checkbuffObj = &(checkbuffCharacter->chBuff.buffList[std::make_tuple(it.dwBuffID, it.nLevel)]);
        if (checkbuffSrcOwn && checkbuffObj->source != self) {
            return false; // buff 来源不是自己, CastSkill 失败
        }
        if (!staticCheckBuffCompare(it.nStackNumCompareFlag, it.nStackNum, checkbuffObj->nStackNum)) {
            return false; // buff 比较不符合要求, CastSkill 失败
        }
    }
    return true;
}

static inline bool staticCheckBuffCompare(int flag, int luaValue, int buffValue) {
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

static bool staticCheckSelfLearntSkill(Character *self, const Skill &skill) {
    for (const auto &it : skill.attrCheckSelfLearntSkill) {
        auto skillLearned = self->chSkill.skillLearned.find(it.dwSkillID);
        if (skillLearned == self->chSkill.skillLearned.end()) {
            return false; // 找不到符合要求的技能, CastSkill 失败
        } else {
            if (!staticCheckSelfLearntSkillCompare(it.nLevelCompareFlag, it.dwSkillLevel, skillLearned->second)) {
                return false; // 技能比较不符合要求, CastSkill 失败
            }
        }
    }
    return true;
}

static inline bool staticCheckSelfLearntSkillCompare(int flag, int luaValue, int skillValue) {
    switch (flag) {
    case static_cast<int>(LuaGlobalTable::SKILL_COMPARE_FLAG::EQUAL):
        return skillValue == luaValue;
        break;
    case static_cast<int>(LuaGlobalTable::SKILL_COMPARE_FLAG::GREATER):
        return skillValue > luaValue;
        break;
    case static_cast<int>(LuaGlobalTable::SKILL_COMPARE_FLAG::GREATER_EQUAL):
        return skillValue >= luaValue;
        break;
    }
    return false;
}

static bool staticCheckCoolDown(Character *self, const Skill &skill) {
    for (const auto &it : skill.attrCoolDown) {
    }
    return true;
}