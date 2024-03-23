#ifndef FRAME_CHARACTER_PROPERTY_SKILLRECIPE_H_
#define FRAME_CHARACTER_PROPERTY_SKILLRECIPE_H_

#include <set>
#include <unordered_map>

namespace jx3calc {
namespace frame {

class SkillRecipe;

class ChSkillRecipe {
public:
    // 下列方法均已在 Character 类中实现.
    // void add(int RecipeID, int RecipeLevel);                                 // 添加秘籍
    // void remove(int RecipeID, int RecipeLevel);                              // 移除秘籍. 实际上是将其标记为失效并从 SkillRecipeTypeMap 和 SkillIDMap 的 std::set 中移除.
    // bool isActive(int RecipeID, int RecipeLevel);                            // 是否激活秘籍
    // std::set<const SkillRecipe *> getList(int SkillID, int SkillRecipeType); // 获取指定技能的秘籍列表. 只会返回有效的秘籍.

    std::unordered_map<int, std::set<const SkillRecipe *>> SkillRecipeTypeMap; // 用于快速查找指定 SkillRecipeType 的 SkillRecipe
    std::unordered_map<int, std::set<const SkillRecipe *>> SkillIDMap;         // 用于快速查找指定 SkillID 的 SkillRecipe
};

} // namespace frame
} // namespace jx3calc

#endif // FRAME_CHARACTER_PROPERTY_SKILLRECIPE_H_
