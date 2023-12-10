#include "frame/global/skill.h"
#include "frame/character/character.h"
#include "frame/character/helper/auto_rollback_attribute.h"
#include "frame/character/helper/runtime_castskill.h"
#include "program/log.h"

using namespace ns_frame;

void Character::skillLearn(int skillID, int skillLevel) {
    this->chSkill.skillLearned[skillID] = skillLevel;
    SkillManager::get(skillID, skillLevel);
}

void Character::skillActive(int skillID) {
    int skillLevel = skillGetLevel(skillID);
    if (skillLevel == 0) {
        return;
    }
    LOG_INFO("ActiveSkill: {} # {}", skillID, skillLevel);
    const Skill &skill = SkillManager::get(skillID, skillLevel);
    RuntimeCastSkill runtime{this, skillID, skillLevel};
    AutoRollbackAttribute *ptr = new AutoRollbackAttribute{this, nullptr, &runtime, skill};
    autoRollbackAttributeList.emplace(ptr);
    this->chSkill.skillActived.emplace(skillID, CharacterSkill::SkillActived{skillLevel, static_cast<void *>(ptr)});
}

void Character::skillDeactive(int skillID) {
    LOG_INFO("DeactiveSkill: {}", skillID);
    auto it = this->chSkill.skillActived.find(skillID);
    if (it != this->chSkill.skillActived.end()) {
        delete static_cast<AutoRollbackAttribute *>(it->second.attribute);
        autoRollbackAttributeList.erase(static_cast<AutoRollbackAttribute *>(it->second.attribute));
        this->chSkill.skillActived.erase(it);
    }
}

void Character::cast(int skillID) {
    int skillLevel = skillGetLevel(skillID);
    if (skillLevel == 0) {
        return;
    }
    this->targetCurr = this->targetSelect;
    skillCast2(skillID, skillLevel);
}

int Character::skillGetLevel(int skillID) {
    if (this->chSkill.skillLearned.find(skillID) == this->chSkill.skillLearned.end()) {
        return 0;
    } else {
        return this->chSkill.skillLearned[skillID];
    }
}