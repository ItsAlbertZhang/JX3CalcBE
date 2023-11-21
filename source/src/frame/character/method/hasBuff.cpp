#include "frame/character/character.h"

using namespace ns_frame;

bool Character::hasBuff(int buffID, int buffLevel) {
    return this->buffExist.find(buffID) != this->buffExist.end() && this->buffExist[buffID].find(buffLevel) != this->buffExist[buffID].end();
}