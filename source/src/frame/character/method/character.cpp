#include "frame/character/character.h"

using namespace ns_frame;

Character::Character() {
    this->dwID = static_cast<int>(characterList.size());
    characterList.push_back(this);
    characterMap.emplace(this, this->dwID);
}

Character *Character::characterGet(int nCharacterID) {
    if (nCharacterID < 0 || nCharacterID >= characterList.size()) {
        return nullptr;
    } else {
        return characterList.at(nCharacterID);
    }
}

int Character::characterGetID(Character *character) {
    if (characterMap.find(character) == characterMap.end()) {
        return -1;
    } else {
        return characterMap.at(character);
    }
}

Character *Character::characterGetSelect() {
    return targetSelect;
}

int Character::characterGetTargetID() {
    return this->targetCurr == nullptr ? 0 : this->targetCurr->dwID;
}