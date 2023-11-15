#include "frame/character.h"

using namespace ns_frame;

int Character::nextCharacterID = 0;

Character::Character() {
    nCharacterID = nextCharacterID++;
}

void Character::setTarget(int targetID) {
    nTargetID = targetID;
}

void Character::AddBuff(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel) {
}