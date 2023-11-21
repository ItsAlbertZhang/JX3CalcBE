#include "frame/character/character.h"

using namespace ns_frame;

Character::Character() {
    nCharacterID = static_cast<int>(characterList.size());
    characterList.push_back(this);
}
