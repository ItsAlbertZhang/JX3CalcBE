#include "frame/character/character.h"

using namespace ns_frame;

Character::Character() {
    this->dwID = static_cast<int>(characterList.size());
    characterList.push_back(this);
    characterMap.emplace(this, this->dwID);
}

Character::~Character() {
    characterList.at(this->dwID) = nullptr;
    characterMap.erase(this);
    // 不能偷懒省掉哈希表的移除, 否则大量 Character 析构紧接构造时, 非常有可能出现:
    // 新构造的对象指针地址与上一个析构的对象指针地址相同. 此时会引发哈希冲突.
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