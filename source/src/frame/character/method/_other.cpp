#include "frame/character/character.h"
#include "frame/event.h"
#include "frame/global/skill.h"

using namespace ns_frame;

void Character::skillLearn(int skillID, int skillLevel) {
    this->chSkill.skillLearned[skillID] = skillLevel;
    SkillManager::get(skillID, skillLevel);
}

Character *Character::characterGet(int nCharacterID) {
    if (nCharacterID < 0 || nCharacterID >= characterList.size()) {
        return nullptr;
    } else {
        return characterList.at(nCharacterID);
    }
}

int Character::characterGetID(Character *character) {
    if (characterMap.find(character) == characterMap.end()) {
        return -1;
    } else {
        return characterMap.at(character);
    }
}

int Character::GetMapID() {
    return 0;
}

int Character::GetKungfuMountID() {
    return this->dwKungfuID;
}

bool Character::IsFormationLeader() {
    return false; // TODO: 暂时未实现阵眼功能
}

void Character::PlayPublicShadowAnimation(int a, int b, bool c, bool d) {
    return;
}

CharacterScene *Character::GetScene() {
    return &(this->chScene);
}

void Character::ClearAllNpcThreat() {
    return;
}

bool Character::IsInParty() {
    return false;
}

void Character::DoAction(int a, int b) {
    return;
}

static void staticCheckSunMoonPower(void *self, void *param) {
    Character *character = (Character *)self;
    if (character->nMoonPowerValue == 1 || character->nSunPowerValue == 1)
        return;
    if (character->nCurrentMoonEnergy >= 10000)
        character->nMoonPowerValue = 1;
    else if (character->nCurrentSunEnergy >= 10000)
        character->nSunPowerValue = 1;
}
void Character::vCheckSunMoonPower() {
    staticCheckSunMoonPower(this, nullptr);
    // Event::add(1024, staticCheckSunMoonPower, this, nullptr);
}

Character *Character::GetSelectCharacter() {
    return targetSelect;
}

bool Character::IsSkillRecipeActive(int RecipeID, int RecipeLevel) {
    return chSkillRecipe.isActive(RecipeID, RecipeLevel);
}

void Character::DestroyPublicShadow(int a, bool b) {
    return;
}

void Character::CreatePublicShadow(int a, int roletype, int dwID, int nX, int nY, int nZ, bool b) {
    return;
}

void Character::skillrecipeAdd(int recipeID, int recipeLevel) {
    chSkillRecipe.add(recipeID, recipeLevel);
}
