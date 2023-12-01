#include "frame/character/character.h"

using namespace ns_frame;

int Character::GetSkillTarget() {
    return this->targetCurr == nullptr ? 0 : this->targetCurr->dwID;
}