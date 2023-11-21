#include "frame/character/character.h"
#include "frame/global/skill.h"

using namespace ns_frame;

void Character::LearnSkill(int skillID, int skillLevel) {
    skillLearned[skillID] = skillLevel;
    SkillManager::get(skillID, skillLevel);
}