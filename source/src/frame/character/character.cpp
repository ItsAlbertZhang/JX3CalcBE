#include "frame/character/character.h"

using namespace ns_frame;

Character::Character() {
    this->dwID = static_cast<int>(characterList.size());
    characterList.push_back(this);
    characterMap.emplace(this, this->dwID);
}
