#include "frame/global/skillrecipe.h"
#include "frame/character/character.h"

using namespace ns_frame;

void Character::skillrecipeAdd(int recipeID, int recipeLevel) {
    const SkillRecipe &skillrecipe = SkillRecipeManager::get(recipeID, recipeLevel);
    this->chSkillRecipe.SkillRecipeTypeMap[skillrecipe.SkillRecipeType].insert(&skillrecipe);
    this->chSkillRecipe.SkillIDMap[skillrecipe.SkillID].insert(&skillrecipe);
}

void Character::skillrecipeRemove(int recipeID, int recipeLevel) {
    const SkillRecipe &skillrecipe = SkillRecipeManager::get(recipeID, recipeLevel);
    this->chSkillRecipe.SkillRecipeTypeMap[skillrecipe.SkillRecipeType].erase(&skillrecipe);
    this->chSkillRecipe.SkillIDMap[skillrecipe.SkillID].erase(&skillrecipe);
}

bool Character::skillrecipeExist(int RecipeID, int RecipeLevel) {
    const SkillRecipe &skillrecipe = SkillRecipeManager::get(RecipeID, RecipeLevel);
    return this->chSkillRecipe.SkillRecipeTypeMap[skillrecipe.SkillRecipeType].find(&skillrecipe) !=
               this->chSkillRecipe.SkillRecipeTypeMap[skillrecipe.SkillRecipeType].end() ||
           this->chSkillRecipe.SkillIDMap[skillrecipe.SkillID].find(&skillrecipe) !=
               this->chSkillRecipe.SkillIDMap[skillrecipe.SkillID].end();
}

std::set<const SkillRecipe *> Character::skillrecipeGet(int skillID, int skillrecipeType) {
    std::set<const SkillRecipe *> emptySet;
    if (skillID != 0) {
        emptySet.insert(
            this->chSkillRecipe.SkillIDMap[skillID].begin(),
            this->chSkillRecipe.SkillIDMap[skillID].end()
        );
    }
    if (skillrecipeType != 0) {
        emptySet.insert(
            this->chSkillRecipe.SkillRecipeTypeMap[skillrecipeType].begin(),
            this->chSkillRecipe.SkillRecipeTypeMap[skillrecipeType].end()
        );
    }
    return emptySet;
}
