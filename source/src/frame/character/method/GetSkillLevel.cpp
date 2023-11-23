#include "frame/character/character.h"

using namespace ns_frame;

int Character::GetSkillLevel(int skillID) {
    if (this->chSkill.skillLearned.find(skillID) == this->chSkill.skillLearned.end()) {
        return 0;
    } else {
        return this->chSkill.skillLearned[skillID];
    }
}