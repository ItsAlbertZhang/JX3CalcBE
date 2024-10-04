#include "frame/character/character.h"
#include "frame/character/helper/skill.h"
#include "frame/common/event.h"
#include "frame/event.h"
#include "frame/global/skill.h"
#include "frame/global/skillrecipe.h"
#include "plugin/log.h"

#define UNREFERENCED_PARAMETER(P) (P)

using namespace jx3calc;
using namespace frame;

void Character::skillCast(int skillID, int skillLevel, int type, int targetID) {
    UNREFERENCED_PARAMETER(type);
    skillCast(characterGet(targetID), skillID, skillLevel);
}

void Character::skillCast(int skillID, int skillLevel, int targetID) {
    skillCast(characterGet(targetID), skillID, skillLevel);
}

void Character::skillCast(int skillID, int skillLevel) {
    if (skillCast(this->targetCurr, skillID, skillLevel)) {
        return;
    }
    // 关于此逻辑, 详见下方注释
    if (skillCast(this->targetSelect, skillID, skillLevel)) {
        this->targetCurr = targetSelect;
        return;
    }
}

void Character::skillCastXYZ(int skillID, int skillLevel, int x, int y, int z) {
    UNREFERENCED_PARAMETER(x);
    UNREFERENCED_PARAMETER(y);
    UNREFERENCED_PARAMETER(z);
    skillCast(this->targetCurr, skillID, skillLevel);
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

static void cb_skill_DelaySubSkill(void *self, void *item);

static inline bool s_check_buff(Character *self, Character *target, const Skill &skill);
static inline bool s_check_buff_compare(int flag, int luaValue, int buffValue);
static inline bool s_check_self_learnt_skill(Character *self, const Skill &skill);
static inline bool s_check_self_learnt_skill_compare(int flag, int luaValue, int skillValue);
// static inline void s_trigger_cooldown(Character *self, int nCoolDownID, int nCoolDownAdd);
static inline auto s_cooldown_left_tick(Character *self, const Skill::SkillCoolDown &cooldown) -> event_tick_t;
static inline auto s_get_cooldown(Character *self, int skillID) -> Skill::SkillCoolDown;

bool Character::skillCast(Character *target, int skillID, int skillLevel) {
    CONSTEXPR_LOG_INFO("Try to CastSkill: {} # {}", skillID, skillLevel);

    // 获取技能
    const Skill &skill = SkillManager::get(skillID, skillLevel);

    // ---------- 进行检查 ----------

    // 1. 检查 tab 中的释放条件

    // 检查门派 ID
    if (skill.MountRequestType > 0 && skill.MountRequestType != mountID)
        return false;
    if (skill.MountRequestDetail > 0 && skill.MountRequestDetail != kungfuID)
        return false;

    // 检查战斗状态
    if (skill.NeedOutOfFight && this->bFightState)
        return false; // 需要处于非战斗状态, 但当前处于战斗状态, CastSkill 失败

    // 检查目标
    if (skill.CastMode == ref::Skill::CastMode::TargetLeader)
        return false; // 不处理阵眼效果
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
    if (!s_check_buff(this, target, skill))
        return false;

    // 2.2 检查自身已学技能
    if (!s_check_self_learnt_skill(this, skill))
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
    std::set<const SkillRecipe *> recipes       = this->skillrecipeGet(skillID, 0);
    std::set<const SkillRecipe *> recipesByType = this->skillrecipeGet(0, skill.RecipeType);
    std::vector<const Skill *>    recipeSkills;
    for (const auto &it : recipes) {
        const Skill *ptrSkill = SkillRecipeManager::getScriptSkill(it, &skill);
        if (nullptr != ptrSkill) {               // 如果技能的秘籍存在对应技能
            recipeSkills.emplace_back(ptrSkill); // 将秘籍的技能加入列表
            // 使用秘籍的技能重载当前的 CD 和 bindbuff
            cooldown.overload(ptrSkill->attrCoolDown);
            bindbuff.overload(ptrSkill->attrBindBuff);
        }
    }
    for (const auto &it : recipesByType) {
        const Skill *ptrSkill = SkillRecipeManager::getScriptSkill(it, &skill);
        if (nullptr != ptrSkill) {               // 如果技能的秘籍存在对应技能
            recipeSkills.emplace_back(ptrSkill); // 将秘籍的技能加入列表
            // 使用 skillrecipeType 获取到的秘籍, 不重载当前的 CD 和 bindbuff.
            // 例如, 无界测试服中, 银月斩通过秘籍的方式重载了 CD, 若子技能也随之重载 CD, 会导致子技能 CD 不满足条件无法释放.
        }
    }
    recipes.insert(recipesByType.begin(), recipesByType.end());

    // 2.5 检查 CD
    if (s_cooldown_left_tick(this, cooldown) > 0)
        return false;

    // ---------- 检查完毕, 释放技能 ----------
    CONSTEXPR_LOG_INFO("{} # {} cast successfully!", skillID, skillLevel);

    // 1. 处理 SkillRecipe: CoolDownAdd, 并处理 damageAddPercent.
    int damageAddPercent = 0;
    for (const auto &it : recipes) {
        cooldown.add(it->CoolDownAdd1, it->CoolDownAdd2, it->CoolDownAdd3);
        damageAddPercent += it->DamageAddPercent; // DamageAddPercent 仅能作用于当前技能.
    }

    // 2. 触发 CD
    if (cooldown.isValidPublicCoolDown) {
        // s_trigger_cooldown(this, cooldown.nPublicCoolDown, 0);
        cooldownModify(cooldown.nPublicCoolDown, 0, 1);
    }
    for (int i = 0; i < 3; i++) {
        if (cooldown.isValidNormalCoolDown[i]) {
            // s_trigger_cooldown(this, cooldown.nNormalCoolDownID[i], cooldown.nNormalCoolDownAdd[i]);
            cooldownModify(cooldown.nNormalCoolDownID[i], cooldown.nNormalCoolDownAdd[i], 1);
        }
    }

    // 3. 构造技能运行时资源: HelperSkill. 它的构造函数会依次:
    //    - 执行 skill event (PreCast)
    //    - 计算会心
    //    - 应用魔法属性
    HelperSkill helper {this, target, nullptr, skill, &recipeSkills, damageAddPercent};

    // 4. 处理其他

    // 4.1 处理日月豆子技能
    if (skill.bIsSunMoonPower) { // 技能是否需要日月豆
        if (this->nSunPowerValue) {
            helper.emplace(skill.SunSubsectionSkillID, skill.SunSubsectionSkillLevel);
            this->nSunPowerValue = 0;
        } else if (this->nMoonPowerValue) {
            helper.emplace(skill.MoonSubsectionSkillID, skill.MoonSubsectionSkillLevel);
            this->nMoonPowerValue = 0;
        }
    }

    // 4.2 处理延迟子技能
    for (auto &it : skill.attrDelaySubSkill) {
        Event::add(it.delay * 1024 / 16, cb_skill_DelaySubSkill, this, static_cast<void *>(const_cast<Skill::DelaySubSkill *>(&it)));
    }

    // 5. 绑定 buff
    const auto bindBuff = [](Character *self, Character *target, const Skill::SkillBindBuff &bindbuff, int skillID, int skillLevel) {
        for (int i = 0; i < 4; i++) {
            if (bindbuff.isValid[i] && target != nullptr) {
                target->buffBind(self->dwID, self->chAttr.atLevel, bindbuff.nBuffID[i], bindbuff.nBuffLevel[i], skillID, skillLevel);
            }
        }
    };
    helper.proxyRecipe(bindBuff, this, target, bindbuff, skillID, skillLevel);

    return true;
    // 6. (自动执行) 析构 helper. 它的析构函数会依次:
    //    - 回滚魔法属性
    //    - 执行 cast skill
    //    - 将 damage 添加至 chDamage
    //    - 执行 skill event (Cast, Hit, 以及视情况 CriticalStrike)
}

event_tick_t Character::skillCooldownLeftTick(int skillID) {
    Skill::SkillCoolDown cooldown = s_get_cooldown(this, skillID);
    return s_cooldown_left_tick(this, cooldown);
}

int Character::skillCountAvailable(int skillID) {
    Skill::SkillCoolDown cooldown = s_get_cooldown(this, skillID);
    int                  ret      = 0;
    for (int i = 0; i < 3; i++) {
        if (cooldown.isValidNormalCoolDown[i] && chCooldown.cooldownList.contains(cooldown.nNormalCoolDownID[i])) {
            const int a = chCooldown.cooldownList.at(cooldown.nNormalCoolDownID[i]).countAvailable;
            if (a > ret) {
                ret = a;
            }
        }
    }
    return ret;
}

static void cb_skill_DelaySubSkill(void *self, void *item) {
    Character                  *ptrSelf = static_cast<Character *>(self);
    const Skill::DelaySubSkill *ptrItem = static_cast<const Skill::DelaySubSkill *>(item);
    ptrSelf->skillCast(ptrItem->skillID, ptrItem->skillLevel);
}

static inline bool s_check_buff(Character *self, Character *target, const Skill &skill) {
    for (const auto &cond : skill.attrCheckBuff) {
        Character *checkbuffCharacter = nullptr;
        bool       checkbuffSrcOwn    = false;
        switch (cond.type) {
        case Skill::SkillCheckBuff::TypeEnum::self:
            checkbuffCharacter = self;
            break;
        case Skill::SkillCheckBuff::TypeEnum::dest:
            checkbuffCharacter = target;
            break;
        case Skill::SkillCheckBuff::TypeEnum::selfOwn:
            checkbuffCharacter = self;
            checkbuffSrcOwn    = true;
            break;
        case Skill::SkillCheckBuff::TypeEnum::destOwn:
            checkbuffCharacter = target;
            checkbuffSrcOwn    = true;
            break;
        }
        if (checkbuffCharacter == nullptr) {
            return false; // 检查 buff 的角色不存在, CastSkill 失败
        }
        BuffItem *buff              = nullptr;
        int       nLevelCompareFlag = cond.nLevelCompareFlag;
        if (cond.nLevel == 0 && cond.nLevelCompareFlag == static_cast<int>(ref::lua::BUFF_COMPARE_FLAG::EQUAL)) {
            /**
             * @note
             * 对于 "要求 buff 不存在" 的检查逻辑, 理论上讲应当是: 对于任何 level 的 buff, 要求其均不存在.
             * 也即: 对于 nLevel GREATER 0 的 buff, 要求其 nStackNum EQUAL 0.
             * 但不知为何, 在实际的 lua 中, 此逻辑变成了: 对于 nLevel EQUAL 0 的 buff, 要求其 nStackNum EQUAL 0. (可见 明教_烈日斩.lua)
             * 因此, 此处对其逻辑进行还原, 将 nLevel 的比较标志设置为 GREATER, 以符合实际的检查逻辑.
             */
            nLevelCompareFlag = static_cast<int>(ref::lua::BUFF_COMPARE_FLAG::GREATER);
        }
        if (checkbuffSrcOwn) {
            buff = checkbuffCharacter->buffGetByOwnerWithCompareFlag(cond.dwBuffID, cond.nLevel, self->dwID, nLevelCompareFlag);
        } else {
            buff = checkbuffCharacter->buffGetWithCompareFlag(cond.dwBuffID, cond.nLevel, nLevelCompareFlag);
        }
        int nStackNum = 0;
        if (nullptr != buff) {
            nStackNum = buff->nStackNum;
        }
        if (!s_check_buff_compare(cond.nStackNumCompareFlag, cond.nStackNum, nStackNum)) {
            return false; // buff 比较不符合要求, CastSkill 失败
        }
    }
    return true;
}

static inline bool s_check_buff_compare(int flag, int luaValue, int buffValue) {
    switch (flag) {
    case static_cast<int>(ref::lua::BUFF_COMPARE_FLAG::EQUAL):
        return buffValue == luaValue;
        break;
    case static_cast<int>(ref::lua::BUFF_COMPARE_FLAG::GREATER):
        return buffValue > luaValue;
        break;
    case static_cast<int>(ref::lua::BUFF_COMPARE_FLAG::GREATER_EQUAL):
        return buffValue >= luaValue;
        break;
    }
    return false;
}

static inline bool s_check_self_learnt_skill(Character *self, const Skill &skill) {
    for (const auto &it : skill.attrCheckSelfLearntSkill) {
        int skillValue = 0; // 技能等级为 0 代表没有学习该技能
        if (self->chSkill.skillLearned.find(it.dwSkillID) != self->chSkill.skillLearned.end()) {
            skillValue = self->chSkill.skillLearned.at(it.dwSkillID); // 获取技能等级
        }
        if (!s_check_self_learnt_skill_compare(it.nLevelCompareFlag, it.dwSkillLevel, skillValue)) {
            return false; // 技能比较不符合要求, CastSkill 失败
        }
    }
    return true;
}

static inline bool s_check_self_learnt_skill_compare(int flag, int luaValue, int skillValue) {
    switch (flag) {
    case static_cast<int>(ref::lua::BUFF_COMPARE_FLAG::EQUAL):
        return skillValue == luaValue; // 其中包含 EQUAL 0 的情况
        break;
    case static_cast<int>(ref::lua::BUFF_COMPARE_FLAG::GREATER):
        return skillValue > luaValue;
        break;
    case static_cast<int>(ref::lua::BUFF_COMPARE_FLAG::GREATER_EQUAL):
        return skillValue >= luaValue;
        break;
    }
    return false;
}

// static inline void s_trigger_cooldown(Character *self, int cooldownID, int cooldownAdd) {
//     const Cooldown &cooldown      = CooldownManager::get(cooldownID);
//     // 计算 CD 时间
//     int             durationFrame = cooldown.DurationFrame * 1024 / (1024 + self->chAttr.getHaste());
//     durationFrame                 = durationFrame > cooldown.MinDurationFrame ? durationFrame : cooldown.MinDurationFrame;
//     durationFrame                 = durationFrame < cooldown.MaxDurationFrame ? durationFrame : cooldown.MaxDurationFrame;
//     durationFrame                 = durationFrame + cooldownAdd;
//     self->cooldownModify(cooldownID, durationFrame);
// }

static inline event_tick_t s_cooldown_left_tick(Character *self, const Skill::SkillCoolDown &cooldown) {
    event_tick_t ret = 0;
    event_tick_t tmp = 0;
    if (cooldown.isValidPublicCoolDown) {
        tmp = self->cooldownLeft(cooldown.nPublicCoolDown);
        ret = tmp > ret ? tmp : ret;
    }
    for (int i = 0; i < 3; i++) {
        if (cooldown.isValidNormalCoolDown[i]) {
            tmp = self->cooldownLeft(cooldown.nNormalCoolDownID[i]);
            ret = tmp > ret ? tmp : ret;
        }
        if (cooldown.isValidCheckCoolDown[i]) {
            tmp = self->cooldownLeft(cooldown.nCheckCoolDownID[i]);
            ret = tmp > ret ? tmp : ret;
        }
    }
    return ret;
}

static inline Skill::SkillCoolDown s_get_cooldown(Character *self, int skillID) {
    // 获取技能
    int skillLevel = self->skillGetLevel(skillID);
    if (skillLevel == 0) {
        return Skill::SkillCoolDown {};
    }
    const Skill                  &skill           = SkillManager::get(skillID, skillLevel);
    Skill::SkillCoolDown          cooldown        = skill.attrCoolDown;
    std::set<const SkillRecipe *> skillrecipeList = self->skillrecipeGet(skillID, 0);
    for (const auto &it : skillrecipeList) {
        const Skill *ptrSkill = SkillRecipeManager::getScriptSkill(it, &skill);
        if (nullptr != ptrSkill) { // 如果技能的秘籍存在对应技能
            // 使用秘籍的技能重载当前的 CD 和 bindbuff
            cooldown.overload(ptrSkill->attrCoolDown);
        }
    }
    return cooldown;
}
