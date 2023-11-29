#include "frame/character/property/skillrecipe.h"
#include "frame/global/skillrecipe.h"
#include <algorithm> // std::set_union

using namespace ns_frame;

void CharacterSkillRecipe::add(int RecipeID, int RecipeLevel) {
    if (skillRecipeList.find(RecipeID) == skillRecipeList.end() || skillRecipeList[RecipeID].find(RecipeLevel) == skillRecipeList[RecipeID].end()) {
        const SkillRecipe &skillrecipe = SkillRecipeManager::get(RecipeID, RecipeLevel);
        skillRecipeList[RecipeID][RecipeLevel] = Item{RecipeID, RecipeLevel, skillrecipe.SkillRecipeType, skillrecipe.SkillID};
    }
    Item *itPtr = &skillRecipeList[RecipeID][RecipeLevel];
    SkillRecipeTypeMap[itPtr->SkillRecipeType].insert(itPtr);
    SkillIDMap[itPtr->SkillID].insert(itPtr);
    itPtr->isValid = true;
}

void CharacterSkillRecipe::remove(int RecipeID, int RecipeLevel) {
    Item *itPtr = &skillRecipeList[RecipeID][RecipeLevel];
    SkillRecipeTypeMap[itPtr->SkillRecipeType].erase(itPtr);
    SkillIDMap[itPtr->SkillID].erase(itPtr);
    itPtr->isValid = false;
}

std::set<CharacterSkillRecipe::Item *> CharacterSkillRecipe::getList(int SkillID, int SkillRecipeType) {
    if (SkillRecipeType == 0) {
        return SkillIDMap[SkillID];
    }
    if (SkillID == 0) {
        return SkillRecipeTypeMap[SkillRecipeType];
    }
    std::set<Item *> emptySet;
    emptySet.merge(SkillIDMap[SkillID]);
    emptySet.merge(SkillRecipeTypeMap[SkillRecipeType]);
    return emptySet;
}