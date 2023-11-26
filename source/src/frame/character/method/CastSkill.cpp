#include "frame/character/character.h"
#include "frame/character/helper/auto_rollback.h"
#include "frame/global/cooldown.h"
#include "frame/global/skill.h"
#include "frame/runtime_lua.h"
#include "frame/static_ref.h"
#include "program/log.h"
#include <random>

using namespace ns_frame;
using namespace ns_framestatic;

static bool staticCheckBuff(Character *self, Character *target, const Skill &skill);
static inline bool staticCheckBuffCompare(int flag, int luaValue, int buffValue);
static bool staticCheckSelfLearntSkill(Character *self, const Skill &skill);
static inline bool staticCheckSelfLearntSkillCompare(int flag, int luaValue, int skillValue);
static bool staticCheckCoolDown(Character *self, const Skill &skill);

void Character::CastSkillTarget(int skillID, int skillLevel, int type, int targetID) {
    AutoRollbackTarget autoRollbackTarget{this, getCharacter(targetID)}; // 自动回滚的目标切换
    CastSkill(skillID, skillLevel);
}

void Character::CastSkill(int skillID, int skillLevel) {
    LOG_INFO("Try to CastSkill: %d # %d\n", skillID, skillLevel);

    // 获取技能
    const Skill &skill = SkillManager::get(skillID, skillLevel);

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

    LOG_INFO("%d # %d cast successfully!\n", skillID, skillLevel);

    // 触发 CD
    for (auto &it : skill.attrCoolDown) {
        if (it.type != Skill::SkillCoolDown::TypeEnum::checkCD) {
            const Cooldown &cooldown = CooldownManager::get(it.nCoolDownID);
            int durationFrame = cooldown.DurationFrame * (1024 - this->chAttr.getHaste()) / 1024;
            durationFrame = durationFrame > cooldown.MinDurationFrame ? durationFrame : cooldown.MinDurationFrame;
            durationFrame = durationFrame < cooldown.MaxDurationFrame ? durationFrame : cooldown.MaxDurationFrame;
            ModifyCoolDown(it.nCoolDownID, durationFrame);
        }
    }
    // 绑定 buff
    for (auto &it : skill.attrBindBuff) {
        target->AddBuff(characterMap[this], this->nLevel, it.nBuffID, it.nBuffLevel); // TODO: 快照属性
    }

    // 计算会心
    auto [atCriticalStrike, atCriticalDamagePower] = CalcCritical(this->chAttr, skillID, skillLevel);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 9999);
    bool isCritical = dis(gen) < atCriticalStrike;

    // 自动回滚的魔法属性
    AutoRollbackAttribute autoRollbackAttribute{this, skill, atCriticalStrike, atCriticalDamagePower, isCritical};
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
    case static_cast<int>(enumLuaBuffCompareFlag::EQUAL):
        return buffValue == luaValue;
        break;
    case static_cast<int>(enumLuaBuffCompareFlag::GREATER):
        return buffValue > luaValue;
        break;
    case static_cast<int>(enumLuaBuffCompareFlag::GREATER_EQUAL):
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
    case static_cast<int>(enumLuaSkillCompareFlag::EQUAL):
        return skillValue == luaValue;
        break;
    case static_cast<int>(enumLuaSkillCompareFlag::GREATER):
        return skillValue > luaValue;
        break;
    case static_cast<int>(enumLuaSkillCompareFlag::GREATER_EQUAL):
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