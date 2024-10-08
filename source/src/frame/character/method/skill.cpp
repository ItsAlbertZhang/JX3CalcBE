#include "frame/global/skill.h"
#include "frame/character/character.h"
#include "frame/character/helper/skill.h"
#include "plugin/log.h"

using namespace jx3calc;
using namespace frame;

bool Character::cast(int skillID) {
    if (skillID <= 0) {
        return false;
    }
    int skillLevel = skillGetLevel(skillID);
    if (skillLevel <= 0) {
        return false;
    }
    this->targetCurr = this->targetSelect;
    return skillCast(targetCurr, skillID, skillLevel);
}

int Character::skillGetLevel(int skillID) {
    if (!this->chSkill.skillLearned.contains(skillID)) {
        return 0;
    } else {
        return this->chSkill.skillLearned[skillID];
    }
}

void Character::skillActive(int skillID) {
    int skillLevel = skillGetLevel(skillID);
    if (skillLevel == 0) {
        return;
    }
    CONSTEXPR_LOG_INFO("ActiveSkill: {} # {}", skillID, skillLevel);
    const Skill &skill = SkillManager::get(skillID, skillLevel);
    HelperSkill *ptr   = new HelperSkill {this, nullptr, nullptr, skill, nullptr, 0};
    autoRollbackAttributeList.emplace(ptr);
    this->chSkill.skillActived.emplace(skillID, ChSkill::SkillActived {skillLevel, static_cast<void *>(ptr)});
}

void Character::skillDeactive(int skillID) {
    CONSTEXPR_LOG_INFO("DeactiveSkill: {}", skillID);
    auto it = this->chSkill.skillActived.find(skillID);
    if (it != this->chSkill.skillActived.end()) {
        delete static_cast<HelperSkill *>(it->second.attribute);
        autoRollbackAttributeList.erase(static_cast<HelperSkill *>(it->second.attribute));
        this->chSkill.skillActived.erase(it);
    }
}

void Character::skillLearn(int skillID, int skillLevel) {
    // skillLevel 可以为 0. 若如此做, 则将技能等级设为最高等级.
    const Skill &it                     = SkillManager::get(skillID, skillLevel);
    this->chSkill.skillLearned[skillID] = it.dwLevel; // skillLevel 为 0 时, it.dwLevel 为最高等级, 否则等于 skillLevel.
    // 将主动技能加入反查表, 以使用技能名施展技能
    if (!it.IsPassiveSkill)
        chSkill.skillRef[it.Name] = skillID;
}
