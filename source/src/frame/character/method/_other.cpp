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

int Character::GetMapID() {
    return 0;
}

int Character::GetKungfuMountID() {
    return this->dwKungfuID;
}

bool Character::IsFormationLeader() {
    return false; // TODO: 暂时未实现阵眼功能
}

void Character::PlayPublicShadowAnimation(int a, int b, bool c, bool d) {
    return;
}

const CharacterScene &Character::GetScene() {
    static thread_local CharacterScene scene{};
    return scene;
}

void Character::ClearAllNpcThreat() {
    return;
}

bool Character::IsInParty() {
    return false;
}

void Character::DoAction(int a, int b) {
    return;
}
