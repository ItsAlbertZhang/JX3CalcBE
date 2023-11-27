#include "frame/character/character.h"

using namespace ns_frame;

int Character::GetSkillTarget() {
    return this->target->dwID;
}