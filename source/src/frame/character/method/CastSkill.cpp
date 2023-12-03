#include "frame/character/character.h"
#include "frame/character/helper/auto_rollback_attribute.h"
#include "frame/character/helper/runtime_castskill.h"
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
static void callbackDelaySubSkill(void *self, void *item);

void Character::CastSkill4(int skillID, int skillLevel, int type, int targetID) {
    CastSkill(getCharacter(targetID), skillID, skillLevel);
}

void Character::CastSkill3(int skillID, int skillLevel, int targetID) {
    CastSkill(getCharacter(targetID), skillID, skillLevel);
}

void Character::CastSkill2(int skillID, int skillLevel) {
    if (CastSkill(this->targetCurr, skillID, skillLevel)) {
        return;
    }
    // 关于此逻辑, 详见下方注释
    if (CastSkill(this->targetSelect, skillID, skillLevel)) {
        this->targetCurr = targetSelect;
        return;
    }
}

void Character::CastSkillXYZ(int skillID, int skillLevel, int x, int y, int z) {
    CastSkill(this->targetCurr, skillID, skillLevel);
    // 关于此逻辑, 详见下方注释
    this->targetCurr = nullptr;
}

/*

在 明教_烈日斩.lua 和 明教_银月斩伤害.lua 中, 有如下内容:

```lua
-- 明教_烈日斩.lua

function GetSkillLevelData(skill)
    --...
    skill.AddAttribute(
        ATTRIBUTE_EFFECT_MODE.EFFECT_TO_SELF_NOT_ROLLBACK, --外功武器伤害
        ATTRIBUTE_TYPE.CAST_SKILL,
        19055,
        2
    );
    --...
end

function Apply(dwCharacterID,dwSkillSrcID)
    --...
    if player.GetSkillLevel(18279) == 1 then
        --处理无武器伤害Bug
        player.CastSkill(19055, 2)

        target.AddBuff(player.dwID, player.nLevel, 12312, 1)
        player.CastSkillXYZ(18280, lv, target.nX, target.nY, target.nZ)
    end
    --...
    if target.GetBuff(4202, 0) then
        player.CastSkill(32816, 2)
    end
    --...
end

```

```lua
-- 明教_银月斩伤害.lua

function GetSkillLevelData(skill)
    --...
    skill.AddAttribute(
        ATTRIBUTE_EFFECT_MODE.EFFECT_TO_SELF_NOT_ROLLBACK, --外功武器伤害
        ATTRIBUTE_TYPE.CAST_SKILL,
        19055,
        8
    );
    --...
end

function Apply(dwCharacterID,dwSkillSrcID)
    --...
    if player.GetSkillLevel(18279) == 1 then
        --处理无武器伤害Bug
        player.CastSkill(19055,8)

        target.AddBuff(player.dwID, player.nLevel, 12312, 1)
        player.CastSkillXYZ(18281, lv, target.nX, target.nY, target.nZ)
    end
    --...
    if target.GetBuff(4202, 0) then
        player.CastSkill(32816, 2)
    end
    --...
end

```

可以看到, 武器伤害技能 (19055) 均出现了两次, 一次出现在 GetSkillLevelData 的 skill.AddAttribute 中 (以下简称 CAST_SKILL), 另一次出现在 Apply 的 player.CastSkill 中 (以下简称 CastSkill).

下面简述游戏内的实际表现:

奇穴 "净身明礼" 刚出现时, 曾有激活其会导致武器伤害失效的 bug 存在. 彼时的 lua 中, 也并无 player.CastSkill(19055) 的存在. 后来, 技能 lua 中添加了 player.CastSkill(19055) 的逻辑, 这一 bug 得以 "修复".

据此可以推测认为, 是 CastSkillXYZ 导致了 CAST_SKILL 的失效.

而在斩触发破招后, CAST_SKILL 又会重新生效, 具体表现为日月斩若 (对 NPC) 触发了破招, 则会多出一次武器伤害. 且伤害的出现顺序为: 外功伤害 - 破招伤害 - 外功伤害 - 内功伤害 - 其他(如净体不畏伤害). 显然, 此处的第一次外功伤害为 CAST_SKILL，第二次则为 CastSkill.

也即, CastSkill会让被 CastSkillXYZ 无效化的 CAST_SKILL 重新生效.

综上, 不妨大胆假设:

1. CastSkillXYZ 会将当前目标置空, 因此其出现导致 CAST_SKILL 无法选取目标而无法释放.
2. CastSkill 会尝试对选择目标进行释放, 若成功释放则将选择目标置为当前目标, 进而使 CAST_SKILL 可以选取目标, 得以释放.

此特性基本不影响计算器功能的实现. 除非日后有更确切的推论, 否则暂以该推测进行代码设计.

*/

bool Character::CastSkill(Character *target, int skillID, int skillLevel) {
    LOG_INFO("\nTry to CastSkill: %d # %d\n", skillID, skillLevel);

    // 获取技能
    const Skill &skill = SkillManager::get(skillID, skillLevel);

    // ---------- 进行检查 ----------

    // 1. 检查 tab 中的释放条件

    // 1.1 检查战斗状态
    if (skill.NeedOutOfFight && this->bFightState)
        return false; // 需要处于非战斗状态, 但当前处于战斗状态, CastSkill 失败

    // 1.2 检查目标
    if ((target == nullptr && !skill.TargetRelationNone) ||
        (target != nullptr && target != this && !skill.TargetRelationEnemy)) {
        if (skill.TargetRelationSelf)
            target = this; // 自动自我运功.
        else
            return false;
    }
    if (target != nullptr) {
        if (target->isPlayer && !skill.TargetTypePlayer)
            return false;
        if (!target->isPlayer && !skill.TargetTypeNpc)
            return false;
    }

    // 2. 检查 lua 中添加的释放条件

    // 2.1 检查 buff
    if (!staticCheckBuff(this, target, skill))
        return false;

    // 2.2 检查自身已学技能
    if (!staticCheckSelfLearntSkill(this, skill))
        return false;

    // 2.3 检查日月豆要求
    if (skill.bIsSunMoonPower) { // 技能是否需要日月豆
        if (0 == this->nSunPowerValue && 0 == this->nMoonPowerValue)
            return false;
    }

    // 2.4 暂停检查, 准备一些后续检查需要的资源
    Skill::SkillCoolDown cooldown = skill.attrCoolDown;
    Skill::SkillBindBuff bindbuff = skill.attrBindBuff;

    // 2.4.1 准备 SkillRecipe (后续的 2.5 步需要用到该资源)
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

    // 2.5 检查 CD
    if (!staticCheckCoolDown(this, cooldown))
        return false;

    // ---------- 检查完毕, 释放技能 ----------
    LOG_INFO("%d # %d cast successfully!\n", skillID, skillLevel);

    // 构造技能运行时资源: RuntimeCastSkill
    RuntimeCastSkill runtime{this, skillID, skillLevel};

    // 1. 执行 SkillEvent: PreCast
    staticTriggerSkillEvent(this, this->chSkillEvent.getList(EventType::PreCast, skillID, skill.SkillEventMask1, skill.SkillEventMask2));

    // 2. 处理 SkillRecipe: CoolDownAdd. 顺便处理 DamageAddPercent.
    int DamageAddPercent = 0;
    for (const auto &it : skillrecipeList) {
        cooldown.add(it->CoolDownAdd1, it->CoolDownAdd2, it->CoolDownAdd3);
        DamageAddPercent += it->DamageAddPercent; // DamageAddPercent 仅能作用于当前技能.
    }

    // 3. 触发 CD
    if (cooldown.isValidPublicCoolDown) {
        staticTriggerCoolDown(this, cooldown.nPublicCoolDown);
    }
    for (int i = 0; i < 3; i++) {
        if (cooldown.isValidNormalCoolDown[i]) {
            staticTriggerCoolDown(this, cooldown.nNormalCoolDownID[i]);
        }
    }

    // 4. 处理魔法属性
    // 构造技能运行时资源: AutoRollbackAttribute
    AutoRollbackAttribute autoRollbackAttribute{this, target, &runtime, skill};

    // 5. 处理其他

    // 5.1 处理日月豆子技能
    if (skill.bIsSunMoonPower) { // 技能是否需要日月豆
        if (this->nSunPowerValue) {
            runtime.skillQueue.emplace(skill.SunSubsectionSkillID, skill.SunSubsectionSkillLevel, this, target);
            this->nSunPowerValue = 0;
        } else if (this->nMoonPowerValue) {
            runtime.skillQueue.emplace(skill.MoonSubsectionSkillID, skill.MoonSubsectionSkillLevel, this, target);
            this->nMoonPowerValue = 0;
        }
    }

    // 5.2 处理延迟子技能
    for (auto &it : skill.attrDelaySubSkill) {
        Event::add(it.delay * 1024 / 16, callbackDelaySubSkill, this, static_cast<void *>(const_cast<Skill::DelaySubSkill *>(&it)));
    }

    // 6. 处理 SkillRecipe: ScriptFile
    // 构造技能运行时资源: vector<AutoRollbackAttribute>
    std::vector<AutoRollbackAttribute> autoRollbackAttributeList;
    for (const auto &it : recipeskillList) {
        autoRollbackAttributeList.emplace_back(this, target, &runtime, *it);
    }

    // 7. 计算伤害
    bool isCritical = autoRollbackAttribute.CallDamage(DamageAddPercent);

    // 8. 绑定 buff
    for (int i = 0; i < 4; i++) {
        if (bindbuff.isValid[i] && target != nullptr) {
            target->BindBuff(dwID, nLevel, bindbuff.nBuffID[i], bindbuff.nBuffLevel[i], skillID, skillLevel);
        }
    }

    // 9. 执行 SkillEvent: Cast, Hit, CriticalStrike
    /* 注:
        1. SkillEvent 的顺序尚不确定.
        2. 其余的 SkillEvent 尚未实现.
        3. 目前 SkillEvent 能够享受 attribute 的加成, 暂时不清楚游戏内是否如此. (因为需要保证 PreCast 的即时插入, 所以 SkillEvent 采取了栈调用的方式)
    */
    staticTriggerSkillEvent(this, this->chSkillEvent.getList(EventType::Cast, skillID, skill.SkillEventMask1, skill.SkillEventMask2));
    staticTriggerSkillEvent(this, this->chSkillEvent.getList(EventType::Hit, skillID, skill.SkillEventMask1, skill.SkillEventMask2));
    if (isCritical) {
        staticTriggerSkillEvent(this, this->chSkillEvent.getList(EventType::CriticalStrike, skillID, skill.SkillEventMask1, skill.SkillEventMask2));
    }

    return true;
    // 10. (自动执行) 析构 SkillRecipe: ScriptFile. 秘籍加成的 attribute 在这里回滚.
    // 11. (自动执行) 析构 魔法属性. 魔法属性的 attribute 在这里回滚.
    // 12. (自动执行) 析构 技能运行时资源: RuntimeCastSkill. 添加至 runtime 的技能队列中的技能被依次执行, 随后添加至 runtime 的伤害被添加至 chDamage.
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
        int skillValue = 0; // 技能等级为 0 代表没有学习该技能
        if (self->chSkill.skillLearned.find(it.dwSkillID) != self->chSkill.skillLearned.end()) {
            skillValue = self->chSkill.skillLearned.at(it.dwSkillID); // 获取技能等级
        }
        if (!staticCheckSelfLearntSkillCompare(it.nLevelCompareFlag, it.dwSkillLevel, skillValue)) {
            return false; // 技能比较不符合要求, CastSkill 失败
        }
    }
    return true;
}

static inline bool staticCheckSelfLearntSkillCompare(int flag, int luaValue, int skillValue) {
    switch (flag) {
    case static_cast<int>(enumLuaSkillCompareFlag::EQUAL):
        return skillValue == luaValue; // 其中包含 EQUAL 0 的情况
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
                caster->CastSkill(target, it->SkillID, it->SkillLevel);
            }
        }
    }
}

static void callbackDelaySubSkill(void *self, void *item) {
    Character *ptrSelf = static_cast<Character *>(self);
    const Skill::DelaySubSkill *ptrItem = static_cast<const Skill::DelaySubSkill *>(item);
    ptrSelf->CastSkill2(ptrItem->skillID, ptrItem->skillLevel);
}

void Character::ActiveSkill(int skillID) {
    int skillLevel = GetSkillLevel(skillID);
    if (skillLevel == 0) {
        return;
    }
    LOG_INFO("\nActiveSkill: %d # %d\n", skillID, skillLevel);
    const Skill &skill = SkillManager::get(skillID, skillLevel);
    RuntimeCastSkill runtime{this, skillID, skillLevel};
    AutoRollbackAttribute *ptr = new AutoRollbackAttribute{this, nullptr, &runtime, skill};
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
    CastSkill2(skillID, skillLevel);
}