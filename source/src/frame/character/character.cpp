#include "frame/character/character.h"

using namespace ns_frame;

Character::Character() {
    nCharacterID = static_cast<int>(characterMap.size());
    characterMap.emplace(this, nCharacterID);
    characterList.push_back(this);
}
