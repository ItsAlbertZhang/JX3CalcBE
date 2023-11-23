#include "frame/character/character.h"
#include "frame/global/skill.h"

using namespace ns_frame;

void Character::LearnSkill(int skillID, int skillLevel) {
    this->chSkill.skillLearned[skillID] = skillLevel;
    SkillManager::get(skillID, skillLevel);
}

Character *Character::getCharacter(int nCharacterID) {
    return characterList[nCharacterID];
}