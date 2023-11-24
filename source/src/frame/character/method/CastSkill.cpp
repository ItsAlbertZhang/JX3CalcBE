#include "frame/character/character.h"
#include "frame/global/cooldown.h"
#include "frame/global/skill.h"
#include "frame/lua_runtime.h"
#include "frame/lua_static.h"
#include "program/log.h"

using namespace ns_frame;

static bool staticCheckBuff(Character *self, Character *target, const Skill &skill);
static inline bool staticCheckBuffCompare(int flag, int luaValue, int buffValue);
static bool staticCheckSelfLearntSkill(Character *self, const Skill &skill);
static inline bool staticCheckSelfLearntSkillCompare(int flag, int luaValue, int skillValue);
static bool staticCheckCoolDown(Character *self, const Skill &skill);

class StaticSkill {
public:
    int atPhysicsDamage = 0;
    int atPhysicsDamageRand = 0;
    int atSolarDamage = 0;
    int atSolarDamageRand = 0;
    int atLunarDamage = 0;
    int atLunarDamageRand = 0;
    int atNeutralDamage = 0;
    int atNeutralDamageRand = 0;
    int atPoisonDamage = 0;
    int atPoisonDamageRand = 0;
};

void Character::CastSkillTarget(int skillID, int skillLevel, int type, int targetID) {
    Character *target = Character::getCharacter(targetID);
    LOG_INFO("Try to CastSkill: %d # %d\n", skillID, skillLevel);
    // 获取技能
    Skill &skill = SkillManager::get(skillID, skillLevel);

    // 检查技能是否可以释放
    if (!staticCheckBuff(this, target, skill)) {
        LOG_INFO("Checkbuff failed!\n");
        return;
    }
    if (!staticCheckSelfLearntSkill(this, skill)) {
        LOG_INFO("SkillCheckSelfLearntSkill failed!\n");
        return;
    }
    if (!staticCheckCoolDown(this, skill)) {
        LOG_INFO("CheckCoolDown failed!\n");
        return;
    }

    CharacterAttr attr = this->chAttr; // 调用拷贝构造函数, 复制一份当前属性的副本. 因此, 在 CastSkill 中, 不会影响到原有的属性.
    StaticSkill staticSkill;
    LOG_INFO("%d # %d cast successfully!\n", skillID, skillLevel);

    // 触发 CD
    for (auto &it : skill.attrCoolDown) {
        if (it.type != Skill::SkillCoolDown::TypeEnum::checkCD) {
            auto &cooldown = CooldownManager::get(it.nCoolDownID);
            ModifyCoolDown(it.nCoolDownID, cooldown.Duration); // TODO: 加速效果还没做
        }
    }
    // 绑定 buff
    for (auto &it : skill.attrBindBuff) {
        target->AddBuff(characterMap[this], this->nLevel, it.nBuffID, it.nBuffLevel);
    }

    // 魔法属性
    for (auto &it : skill.attrAttributes) {
        switch (it.type) {
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::EXECUTE_SCRIPT): {
            std::string paramStr = "scripts/" + it.param1Str;
            int dwCharacterID = characterMap[target];
            int dwSkillSrcID = characterMap[this];
            LuaFunc::analysis(LuaFunc::getApply(paramStr)(dwCharacterID, dwSkillSrcID), paramStr, LuaFunc::Enum::Apply);
        } break;
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::EXECUTE_SCRIPT_WITH_PARAM): {
            std::string paramStr = "scripts/" + it.param1Str;
            int dwCharacterID = characterMap[target];
            int dwSkillSrcID = characterMap[this];
            LuaFunc::analysis(LuaFunc::getApply(paramStr)(dwCharacterID, it.param2, dwSkillSrcID), paramStr, LuaFunc::Enum::Apply);
        } break;
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::CAST_SKILL_TARGET_DST):
            this->chSkill.skillQueue.emplace(it.param1Int, it.param2);
            break;
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::CAST_SKILL):
            this->chSkill.skillQueue.emplace(it.param1Int, it.param2);
            break;
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::SKILL_PHYSICS_DAMAGE):
            staticSkill.atPhysicsDamage = it.param1Int;
            break;
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::SKILL_PHYSICS_DAMAGE_RAND):
            staticSkill.atPhysicsDamageRand = it.param1Int;
            break;
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::SKILL_SOLAR_DAMAGE):
            staticSkill.atSolarDamage = it.param1Int;
            break;
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::SKILL_SOLAR_DAMAGE_RAND):
            staticSkill.atSolarDamageRand = it.param1Int;
            break;
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::SKILL_LUNAR_DAMAGE):
            staticSkill.atLunarDamage = it.param1Int;
            break;
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::SKILL_LUNAR_DAMAGE_RAND):
            staticSkill.atLunarDamageRand = it.param1Int;
            break;
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::SKILL_NEUTRAL_DAMAGE):
            staticSkill.atNeutralDamage = it.param1Int;
            break;
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::SKILL_NEUTRAL_DAMAGE_RAND):
            staticSkill.atNeutralDamageRand = it.param1Int;
            break;
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::SKILL_POISON_DAMAGE):
            staticSkill.atPoisonDamage = it.param1Int;
            break;
        case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::SKILL_POISON_DAMAGE_RAND):
            staticSkill.atPoisonDamageRand = it.param1Int;
            break;

        default:
            LOG_ERROR("Undefined type: %d %s: %d %d\n", it.type, LuaTableString::luaAttributeType[it.type], it.param1Int, it.param2);
        }
    }

    // 执行队列中的剩余技能
    while (!this->chSkill.skillQueue.empty()) {
        auto it = this->chSkill.skillQueue.front();
        this->chSkill.skillQueue.pop();
        this->CastSkill(it.skillID, it.skillLevel);
    }
}

void Character::CastSkill(int skillID, int skillLevel) {
    CastSkillTarget(skillID, skillLevel, static_cast<int>(target->isPlayer), target->dwID);
}

static bool staticCheckBuff(Character *self, Character *target, const Skill &skill) {
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
            checkbuffCharacter = target;
            break;
        case Skill::SkillCheckBuff::TypeEnum::selfOwn:
            checkbuffCharacter = self;
            checkbuffSrcOwn = true;
            break;
        case Skill::SkillCheckBuff::TypeEnum::destOwn:
            checkbuffCharacter = target;
            checkbuffSrcOwn = true;
            break;
        }
        if (!checkbuffCharacter->IsHaveBuff(it.dwBuffID, it.nLevel)) {
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
        if (self->chCooldown.cooldownList.find(it.nCoolDownID) != self->chCooldown.cooldownList.end() && self->chCooldown.cooldownList[it.nCoolDownID].isValid) {
            // 在 CD 列表中找到了该 CD, 且 isValid 处于激活状态
            return false;
            // TODO: 充能技能尚未实现. 即使 CD 有效, 也有可能能够 CastSkill
        }
    }
    return true;
}