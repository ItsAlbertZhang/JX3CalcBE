#include "frame/character/character.h"

using namespace ns_frame;

bool Character::hasBuff(int buffID, int buffLevel) {
    return this->chBuff.buffList.find(std::make_tuple(buffID, buffLevel)) != this->chBuff.buffList.end();
}