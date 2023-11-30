#include "frame/character/property/skillrecipe.h"

using namespace ns_frame;

void CharacterSkillRecipe::add(int RecipeID, int RecipeLevel) {
    const SkillRecipe &skillrecipe = SkillRecipeManager::get(RecipeID, RecipeLevel);
    SkillRecipeTypeMap[skillrecipe.SkillRecipeType].insert(&skillrecipe);
    SkillIDMap[skillrecipe.SkillID].insert(&skillrecipe);
}

void CharacterSkillRecipe::remove(int RecipeID, int RecipeLevel) {
    const SkillRecipe &skillrecipe = SkillRecipeManager::get(RecipeID, RecipeLevel);
    SkillRecipeTypeMap[skillrecipe.SkillRecipeType].erase(&skillrecipe);
    SkillIDMap[skillrecipe.SkillID].erase(&skillrecipe);
}

std::set<const SkillRecipe *> CharacterSkillRecipe::getList(int SkillID, int SkillRecipeType) {
    std::set<const SkillRecipe *> emptySet;
    if (SkillID != 0) {
        emptySet.merge(SkillIDMap[SkillID]);
    }
    if (SkillRecipeType != 0) {
        emptySet.merge(SkillRecipeTypeMap[SkillRecipeType]);
    }
    return emptySet;
}