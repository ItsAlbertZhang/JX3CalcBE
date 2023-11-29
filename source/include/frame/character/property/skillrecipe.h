#ifndef FRAME_CHARACTER_PROPERTY_SKILLRECIPE_H_
#define FRAME_CHARACTER_PROPERTY_SKILLRECIPE_H_

#include <set>
#include <unordered_map>

namespace ns_frame {

class CharacterSkillRecipe {
public:
    /**
     * @brief 单个 SkillRecipe
     */
    class Item {
    public:
        Item() = default;
        Item(int RecipeID, int RecipeLevel, int SkillRecipeType, int SkillID)
            : RecipeID(RecipeID), RecipeLevel(RecipeLevel), SkillRecipeType(SkillRecipeType), SkillID(SkillID) {}
        bool isValid = false;
        int RecipeID;
        int RecipeLevel;
        int SkillRecipeType;
        int SkillID;
    };

    void add(int RecipeID, int RecipeLevel);                    // 添加秘籍
    void remove(int RecipeID, int RecipeLevel);                 // 移除秘籍. 实际上是将其标记为失效并从 SkillRecipeTypeMap 和 SkillIDMap 的 std::set 中移除.
    std::set<Item *> getList(int SkillID, int SkillRecipeType); // 获取指定技能的秘籍列表. 只会返回有效的秘籍.

private:
    /**
     * @brief SkillRecipe 列表
     */
    std::unordered_map<int, std::unordered_map<int, Item>> skillRecipeList;
    std::unordered_map<int, std::set<Item *>> SkillRecipeTypeMap; // 用于快速查找指定 SkillRecipeType 的 SkillRecipe
    std::unordered_map<int, std::set<Item *>> SkillIDMap;         // 用于快速查找指定 SkillID 的 SkillRecipe
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_PROPERTY_SKILLRECIPE_H_