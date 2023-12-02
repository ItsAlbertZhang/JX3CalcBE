#include "frame/character/character.h"
#include "frame/character/helper/auto_rollback_attribute.h"
#include "frame/character/helper/auto_rollback_target.h"
#include "frame/character/helper/skill_runtime.h"
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
    AutoRollbackTarget autoRollbackTarget{this}; // 自动回滚的目标切换
    this->targetCurr = getCharacter(targetID);
    CastSkill(skillID, skillLevel);
}

void Character::CastSkillXYZ(int skillID, int skillLevel, int x, int y, int z) {
    /**
     * 猜测实现:
     * 现象: 明教在激活奇穴"净身明礼"的情况下, 烈日斩和银月斩会调用 CastSkillXYZ, 与此同时 AddAttribute 中的 CAST_SKILL (武器伤害) 失效.
     * 推测原因: CastSkillXYZ 会清空上一个 SkillRuntime 的技能队列. (虽然很离谱, 但是似乎没有别的更好的解释)
     * 因此, 此处将其置空, 以还原类似游戏内的逻辑. 除非更确切地知道了游戏内的实现逻辑, 否则不建议修改此处.
     */
    SkillRuntime *ptr = SkillRuntime::runtimeStack.top();
    std::queue<ns_frame::SkillRuntime::SkillQueueElement> empty;
    ptr->skillQueue.swap(empty);
    CastSkill(skillID, skillLevel);
}

void Character::CastSkill(int skillID, int skillLevel) {
    LOG_INFO("\nTry to CastSkill: %d # %d\n", skillID, skillLevel);

    // 获取技能
    const Skill &skill = SkillManager::get(skillID, skillLevel);

    // 检查 tab 中的释放条件
    if (skill.NeedOutOfFight && !this->isOutOfFight)
        return; // 需要处于非战斗状态, 但当前处于战斗状态, CastSkill 失败

    AutoRollbackTarget autoRollbackTarget{this}; // 自动回滚的目标切换
    if ((this->targetCurr == nullptr && !skill.TargetRelationNone) ||
        (this->targetCurr != nullptr && this->targetCurr != this && !skill.TargetRelationEnemy)) {
        if (skill.TargetRelationSelf)
            this->targetCurr = this; // 自动自我运功.
        else
            return;
    }
    if (this->targetCurr != nullptr) {
        if (this->targetCurr->isPlayer && !skill.TargetTypePlayer)
            return;
        if (!this->targetCurr->isPlayer && !skill.TargetTypeNpc)
            return;
    }

    // 检查 lua 中添加的释放条件
    if (!staticCheckBuff(this, targetCurr, skill))
        return;

    if (!staticCheckSelfLearntSkill(this, skill))
        return;

    if (skill.bIsSunMoonPower) { // 技能是否需要日月豆
        if (0 == this->nSunPowerValue && 0 == this->nMoonPowerValue)
            return;
    }

    // 暂停检查, 准备一些资源
    Skill::SkillCoolDown cooldown = skill.attrCoolDown;
    Skill::SkillBindBuff bindbuff = skill.attrBindBuff;

    // 准备 SkillRecipe: 配合检查
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

    // 继续检查 CD
    if (!staticCheckCoolDown(this, cooldown))
        return;

    // 检查完毕, 可以释放技能
    LOG_INFO("%d # %d cast successfully!\n", skillID, skillLevel);
    SkillRuntime runtime{this, skillID, skillLevel};

    // 处理 SkillEvent: PreCast
    staticTriggerSkillEvent(this, this->chSkillEvent.getList(EventType::PreCast, skillID, skill.SkillEventMask1, skill.SkillEventMask2));

    // 处理 SkillRecipe: CoolDownAdd 和 DamageAddPercent.
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

    // 自动回滚的魔法属性
    AutoRollbackAttribute autoRollbackAttribute{this, &runtime, skill};

    if (skill.bIsSunMoonPower) { // 技能是否需要日月豆
        if (this->nSunPowerValue) {
            runtime.skillQueue.emplace(skill.SunSubsectionSkillID, skill.SunSubsectionSkillLevel, this, this->targetCurr);
        } else { // 不需要再判断, 因为 nSunPowerValue 和 nMoonPowerValue 不可能同时为 0 (那样在前面就 return 了)
            runtime.skillQueue.emplace(skill.MoonSubsectionSkillID, skill.MoonSubsectionSkillLevel, this, this->targetCurr);
        }
    }

    // 处理 SkillRecipe: skill 的 Attribute. 秘籍加成的双会可以作用于绑定 buff.
    std::vector<AutoRollbackAttribute> autoRollbackAttributeList;
    for (const auto &it : recipeskillList) {
        autoRollbackAttributeList.emplace_back(this, &runtime, *it);
    }

    // 计算伤害
    bool isCritical = autoRollbackAttribute.CallDamage(DamageAddPercent);

    // 绑定 buff. 其晚于计算伤害, 直观佐证为日斩无法享受其 BindBuff 的加成.
    for (int i = 0; i < 4; i++) {
        if (bindbuff.isValid[i] && this->targetCurr != nullptr) {
            this->targetCurr->BindBuff(dwID, nLevel, bindbuff.nBuffID[i], bindbuff.nBuffLevel[i], skillID, skillLevel);
        }
    }

    /* SkillEvent: Cast & Hit. 注:
    1. SkillEvent 的顺序尚不确定.
    2. 其余的 SkillEvent 尚未实现.
    3. 目前 SkillEvent 能够享受 attribute 的加成, 暂时不清楚游戏内是否如此. (因为需要保证 PreCast 的即时插入, 所以 SkillEvent 采取了栈调用的方式)
    */
    staticTriggerSkillEvent(this, this->chSkillEvent.getList(EventType::Cast, skillID, skill.SkillEventMask1, skill.SkillEventMask2));
    staticTriggerSkillEvent(this, this->chSkillEvent.getList(EventType::Hit, skillID, skill.SkillEventMask1, skill.SkillEventMask2));
    if (isCritical) {
        staticTriggerSkillEvent(this, this->chSkillEvent.getList(EventType::CriticalStrike, skillID, skill.SkillEventMask1, skill.SkillEventMask2));
    }

    // 析构顺序: autoRollbackAttribute (回滚当前技能 lua 的 GetSkillLevelData)
    //       -> autoRollbackAttributeList (回滚秘籍 lua 的 GetSkillLevelData)
    //       -> runtime (释放技能队列并添加伤害)
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
        if (checkbuffCharacter == nullptr) {
            return false; // 检查 buff 的角色不存在, CastSkill 失败
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
            buff = checkbuffCharacter->GetBuffByOwnerWithCompareFlag(cond.dwBuffID, cond.nLevel, self->dwID, nLevelCompareFlag);
        } else {
            buff = checkbuffCharacter->GetBuffWithCompareFlag(cond.dwBuffID, cond.nLevel, nLevelCompareFlag);
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
            Character *caster = it->SkillCaster == EventCT::EventCaster ? self : self->targetCurr;
            Character *target = it->SkillTarget == EventCT::EventTarget ? self->targetCurr : self;
            if (caster != nullptr) {
                caster->CastSkillTarget(
                    it->SkillID, it->SkillLevel,
                    target == nullptr  ? 0
                    : target->isPlayer ? static_cast<int>(CharacterType::PLAYER)
                                       : static_cast<int>(CharacterType::NPC),
                    target == nullptr ? 0 : target->dwID);
            }
        }
    }
}

void Character::ActiveSkill(int skillID) {
    int skillLevel = GetSkillLevel(skillID);
    if (skillLevel == 0) {
        return;
    }
    LOG_INFO("\nActiveSkill: %d # %d\n", skillID, skillLevel);
    const Skill &skill = SkillManager::get(skillID, skillLevel);
    SkillRuntime runtime{this, skillID, skillLevel};
    AutoRollbackAttribute *ptr = new AutoRollbackAttribute{this, &runtime, skill};
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

void Character::Cast(int skillID) {
    int skillLevel = GetSkillLevel(skillID);
    if (skillLevel == 0) {
        return;
    }
    this->targetCurr = this->targetSelect;
    CastSkill(skillID, skillLevel);
}