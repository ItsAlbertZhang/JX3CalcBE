#include "frame/character/character.h"

using namespace ns_frame;

CharacterBuff::Item *Character::GetBuff(int buffID, int buffLevel) {
    if (this->chBuff.buffList.find(std::make_tuple(buffID, buffLevel)) == this->chBuff.buffList.end()) {
        return nullptr;
    } else {
        return &this->chBuff.buffList[std::make_tuple(buffID, buffLevel)];
    }
}