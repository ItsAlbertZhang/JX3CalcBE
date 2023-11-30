#include "frame/character/character.h"
#include "frame/character/helper/auto_rollback_attribute.h"
#include "frame/character/helper/auto_rollback_target.h"
#include "frame/global/cooldown.h"
#include "frame/global/skill.h"
#include "frame/global/skillrecipe.h"
#include "frame/runtime_lua.h"
#include "frame/static_ref.h"
#include "program/log.h"
#include <random>

using namespace ns_frame;
using namespace ns_framestatic;

static inline bool staticCheckBuff(Character *self, Character *target, const Skill &skill);
static inline bool staticCheckBuffCompare(int flag, int luaValue, int buffValue);
static inline bool staticCheckSelfLearntSkill(Character *self, const Skill &skill);
static inline bool staticCheckSelfLearntSkillCompare(int flag, int luaValue, int skillValue);
static inline bool staticCheckCoolDown(Character *self, const Skill::SkillCoolDown &cooldown);
static inline void staticTriggerCoolDown(Character *self, int nCoolDownID);
static inline void staticTriggerSkillEvent(Character *self, const std::set<const SkillEvent *> &skillevent);

void Character::CastSkillTarget(int skillID, int skillLevel, int type, int targetID) {
    AutoRollbackTarget autoRollbackTarget{this, getCharacter(targetID)}; // 自动回滚的目标切换
    CastSkill(skillID, skillLevel);
}

void Character::CastSkill(int skillID, int skillLevel) {
    LOG_INFO("\nTry to CastSkill: %d # %d\n", skillID, skillLevel);

    // 获取技能
    const Skill &skill = SkillManager::get(skillID, skillLevel);

    // 检查 tab 中的释放条件
    if (skill.NeedOutOfFight && !this->isOutOfFight) {
        LOG_INFO("Check failed: NeedOutOfFight!\n%s", "");
        return;
    }
    if (this->target->isPlayer && !skill.TargetTypePlayer) {
        LOG_INFO("Check failed: TargetTypePlayer!\n%s", "");
        return;
    }
    if (!this->target->isPlayer && !skill.TargetTypeNpc) {
        LOG_INFO("Check failed: TargetTypeNpc!\n%s", "");
        return;
    }

    // 检查 lua 中添加的释放条件
    if (!staticCheckBuff(this, target, skill)) {
        LOG_INFO("CheckBuff failed!\n%s", "");
        return;
    }
    if (!staticCheckSelfLearntSkill(this, skill)) {
        LOG_INFO("CheckSelfLearntSkill failed!\n%s", "");
        return;
    }

    Skill::SkillCoolDown cooldown = skill.attrCoolDown;
    Skill::SkillBindBuff bindbuff = skill.attrBindBuff;

    // SkillRecipe: 准备及配合检查
    std::set<const SkillRecipe *> skillrecipeList = this->chSkillRecipe.getList(skillID, skill.RecipeType);
    std::vector<const Skill *> recipeskillList;
    for (const auto &it : skillrecipeList) {
        const Skill *ptrSkill = SkillRecipeManager::getSkill(it);
        if (nullptr != ptrSkill) {                  // 如果技能的秘籍存在对应技能
            recipeskillList.emplace_back(ptrSkill); // 将秘籍的技能加入列表
            // 使用秘籍的技能重载当前的 CD 和 bindbuff
            cooldown.overload(ptrSkill->attrCoolDown);
            bindbuff.overload(ptrSkill->attrBindBuff);
        }
    }

    if (!staticCheckCoolDown(this, cooldown)) {
        LOG_INFO("CheckCoolDown failed!\n%s", "");
        return;
    }

    // 检查完毕, 可以释放技能
    LOG_INFO("%d # %d cast successfully!\n", skillID, skillLevel);

    // SkillEvent: PreCast
    staticTriggerSkillEvent(this, this->chSkillEvent.getList(EventType::PreCast, skillID, skill.SkillEventMask1, skill.SkillEventMask2));

    // SkillRecipe: skill 的 Attribute. 秘籍加成的双会可以作用于绑定 buff.
    std::vector<AutoRollbackAttribute> autoRollbackAttributeList;
    for (const auto &it : recipeskillList) {
        autoRollbackAttributeList.emplace_back(this, *it, 0, 0, false, 0);
        // 当前函数结束时, autoRollbackAttributeList 会被析构, 从而自动回滚.
    }
    // SkillRecipe: CoolDownAdd 和 DamageAddPercent.
    int DamageAddPercent = 0;
    for (const auto &it : skillrecipeList) {
        cooldown.add(it->CoolDownAdd1, it->CoolDownAdd2, it->CoolDownAdd3);
        DamageAddPercent += it->DamageAddPercent; // DamageAddPercent 仅能作用于当前技能.
    }

    // 触发 CD
    if (cooldown.isValidPublicCoolDown) {
        staticTriggerCoolDown(this, cooldown.nPublicCoolDown);
    }
    for (int i = 0; i < 3; i++) {
        if (cooldown.isValidNormalCoolDown[i]) {
            staticTriggerCoolDown(this, cooldown.nNormalCoolDownID[i]);
        }
    }

    // 绑定 buff
    for (int i = 0; i < 4; i++) {
        if (bindbuff.isValid[i]) {
            target->BindBuff(dwID, nLevel, bindbuff.nBuffID[i], bindbuff.nBuffLevel[i], skillID, skillLevel);
        }
    }

    // 计算会心
    auto [atCriticalStrike, atCriticalDamagePower] = CalcCritical(this->chAttr, skillID, skillLevel);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 9999);
    bool isCritical = dis(gen) < atCriticalStrike;

    // 自动回滚的魔法属性
    AutoRollbackAttribute autoRollbackAttribute{this, skill, atCriticalStrike, atCriticalDamagePower, isCritical, DamageAddPercent};

    // SkillEvent: Cast & Hit
    staticTriggerSkillEvent(this, this->chSkillEvent.getList(EventType::Cast, skillID, skill.SkillEventMask1, skill.SkillEventMask2));
    staticTriggerSkillEvent(this, this->chSkillEvent.getList(EventType::Hit, skillID, skill.SkillEventMask1, skill.SkillEventMask2));

    // 注: 其余的 SkillEvent 尚未实现.
}

static inline bool staticCheckBuff(Character *self, Character *target, const Skill &skill) {
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

static inline bool staticCheckSelfLearntSkill(Character *self, const Skill &skill) {
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

static inline bool staticCheckCoolDown(Character *self, const Skill::SkillCoolDown &cooldown) {
    // TODO: 充能技能尚未实现. 即使 CD 有效, 也有可能能够 CastSkill
    if (cooldown.isValidPublicCoolDown) {
        if (self->chCooldown.cooldownList.find(cooldown.nPublicCoolDown) != self->chCooldown.cooldownList.end() &&
            self->chCooldown.cooldownList[cooldown.nPublicCoolDown].isValid) {
            return false; // CD 存在于列表中且未冷却完毕, CastSkill 失败
        }
    }
    for (int i = 0; i < 3; i++) {
        if (cooldown.isValidNormalCoolDown[i]) {
            if (self->chCooldown.cooldownList.find(cooldown.nNormalCoolDownID[i]) != self->chCooldown.cooldownList.end() &&
                self->chCooldown.cooldownList[cooldown.nNormalCoolDownID[i]].isValid) {
                return false; // CD 存在于列表中且未冷却完毕, CastSkill 失败
            }
        }
        if (cooldown.isValidCheckCoolDown[i]) {
            if (self->chCooldown.cooldownList.find(cooldown.nCheckCoolDownID[i]) != self->chCooldown.cooldownList.end() &&
                self->chCooldown.cooldownList[cooldown.nCheckCoolDownID[i]].isValid) {
                return false; // CD 存在于列表中且未冷却完毕, CastSkill 失败
            }
        }
    }
    return true;
}

static inline void staticTriggerCoolDown(Character *self, int cooldownID) {
    const Cooldown &cooldown = CooldownManager::get(cooldownID);
    int durationFrame = cooldown.DurationFrame * (1024 - self->chAttr.getHaste()) / 1024;
    durationFrame = durationFrame > cooldown.MinDurationFrame ? durationFrame : cooldown.MinDurationFrame;
    durationFrame = durationFrame < cooldown.MaxDurationFrame ? durationFrame : cooldown.MaxDurationFrame;
    self->ModifyCoolDown(cooldownID, durationFrame);
}

static inline void staticTriggerSkillEvent(Character *self, const std::set<const SkillEvent *> &skillevent) {
    for (const auto &it : skillevent) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 1023);
        if (dis(gen) < it->Odds) {
            Character *caster = it->SkillCaster == EventCT::EventCaster ? self : self->target;
            Character *target = it->SkillTarget == EventCT::EventTarget ? self->target : self;
            caster->CastSkillTarget(
                it->SkillID, it->SkillLevel,
                target->isPlayer
                    ? static_cast<int>(ns_framestatic::enumLuaTarget::PLAYER)
                    : static_cast<int>(ns_framestatic::enumLuaTarget::NPC),
                target->dwID);
        }
    }
}

void Character::ActiveSkill(int skillID, int skillLevel) {
    LOG_INFO("\nActiveSkill: %d # %d\n", skillID, skillLevel);
    const Skill &skill = SkillManager::get(skillID, skillLevel);
    AutoRollbackAttribute *ptr = new AutoRollbackAttribute{this, skill, 0, 0, false, 0};
    // this->chSkill.skillActived[skillID] = CharacterSkill::SkillActived{skillLevel, static_cast<void *>(ptr)};
    this->chSkill.skillActived.emplace(skillID, CharacterSkill::SkillActived{skillLevel, static_cast<void *>(ptr)});
}

void Character::DeactiveSkill(int skillID) {
    LOG_INFO("\nDeactiveSkill: %d\n", skillID);
    auto it = this->chSkill.skillActived.find(skillID);
    if (it != this->chSkill.skillActived.end()) {
        delete static_cast<AutoRollbackAttribute *>(it->second.attribute);
        this->chSkill.skillActived.erase(it);
    }
}