#include "frame/character.h"
#include "frame/skill.h"

using namespace frame;

int Character::nextCharacterID = 0;

Character::Character() {
    nCharacterID = nextCharacterID++;
}

void Character::setTarget(int targetID) {
    nTargetID = targetID;
}

void Character::LearnSkill(int skillID, int skillLevel) {
    skillLearned[skillID] = skillLevel;
    // CacheSkill::init(skillID, skillLevel);
}

void Character::CastSkill(int skillID, int skillLevel) {
}

void Character::AddBuff(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel) {
}