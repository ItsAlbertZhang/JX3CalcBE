#include "frame/character/character.h"
#include "frame/character/helper/auto_rollback_attrib.h"
#include "frame/character/helper/auto_rollback_attribute.h"
#include <cstdlib>

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
    for (auto &it : autoRollbackAttribList) {
        std::free(it); // 没必要析构和置空, 直接 free 省点性能
    }
    for (auto &it : autoRollbackAttributeList) {
        std::free(it); // 没必要析构和置空, 直接 free 省点性能
    }
}

Character *Character::characterGet(int nCharacterID) {
    if (nCharacterID < 0 || nCharacterID >= static_cast<int>(characterList.size())) {
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
