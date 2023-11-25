#include "frame/character/character.h"
#include "frame/global/skill.h"

using namespace ns_frame;

void Character::LearnSkill(int skillID, int skillLevel) {
    this->chSkill.skillLearned[skillID] = skillLevel;
    SkillManager::get(skillID, skillLevel);
}

Character *Character::getCharacter(int nCharacterID) {
    if (nCharacterID < 0 || nCharacterID >= characterList.size()) {
        return nullptr;
    } else {
        return characterList.at(nCharacterID);
    }
}

int Character::getCharacterID(Character *character) {
    if (characterMap.find(character) == characterMap.end()) {
        return -1;
    } else {
        return characterMap.at(character);
    }
}