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
    LOG_INFO("\nTry to CastSkill: %d # %d\n", skillID, skillLevel);

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
    for (const auto &cond : skill.attrCheckBuff) {
        Character *checkbuffCharacter = nullptr;
        bool checkbuffSrcOwn = false;
        switch (cond.type) {
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
        CharacterBuff::Item *buff = nullptr;
        int nLevelCompareFlag = cond.nLevelCompareFlag;
        if (cond.nLevel == 0 && cond.nLevelCompareFlag == static_cast<int>(enumLuaBuffCompareFlag::EQUAL)) {
            /**
             * @note
             * 对于 "要求 buff 不存在" 的检查逻辑, 理论上讲应当是: 对于任何 level 的 buff, 要求其均不存在.
             * 也即: 对于 nLevel GREATER 0 的 buff, 要求其 nStackNum EQUAL 0.
             * 但不知为何, 在实际的 lua 中, 此逻辑变成了: 对于 nLevel EQUAL 0 的 buff, 要求其 nStackNum EQUAL 0. (可见 明教_烈日斩.lua)
             * 因此, 此处对其逻辑进行还原, 将 nLevel 的比较标志设置为 GREATER, 以符合实际的检查逻辑.
             */
            nLevelCompareFlag = static_cast<int>(enumLuaBuffCompareFlag::GREATER);
        }
        if (checkbuffSrcOwn) {
            buff = checkbuffCharacter->GetBuffByOwnerWithCompareFlag(cond.dwBuffID, cond.nLevel, self->dwID, cond.nLevelCompareFlag);
        } else {
            buff = checkbuffCharacter->GetBuffWithCompareFlag(cond.dwBuffID, cond.nLevel, cond.nLevelCompareFlag);
        }
        int nStackNum = 0;
        if (nullptr != buff) {
            nStackNum = buff->nStackNum;
        }
        if (!staticCheckBuffCompare(cond.nStackNumCompareFlag, cond.nStackNum, nStackNum)) {
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