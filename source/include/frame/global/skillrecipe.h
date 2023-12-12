#ifndef FRAME_GLOBAL_SKILLRECIPE_H_
#define FRAME_GLOBAL_SKILLRECIPE_H_

#include "frame/global/skill.h"
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace ns_frame {

/**
 * @brief SkillRecipe 类
 * @warning SkillRecipe 是公共资源, 不属于某个角色. 应当使用 SkillRecipeManager 类对其统一进行存取.
 * @warning #### 关于线程安全以及实现方式:
 * @warning - 这个类的实例会在多线程之间共享.
 * @warning - 在构造以外的情况下, 这个类的实例 **应当是** 只读的.
 * @warning - 这个类实例的构造由 `Manager::add()` 完成, 并借助锁确保线程安全.
 */
class SkillRecipe {
public:
    std::unordered_map<std::string, std::string> tab; // buffs.tab 中的数据

    // 初始化时拿出一些数据, 降低使用开销
    int RecipeID;
    int RecipeLevel;
    int SkillRecipeType;
    int SkillID;
    int CoolDownAdd1;
    int CoolDownAdd2;
    int CoolDownAdd3;
    int DamageAddPercent;
    // std::string ScriptFile; // ScriptFile 不在此处实现, 而是在 SkillManager::getRecipe() 中实现
};

/**
 * @brief SkillRecipe 管理类
 * @warning 这是一个静态类, 用于管理 SkillRecipe 数据, 不应当被创建实例.
 */
class SkillRecipeManager {
public:
    SkillRecipeManager() = delete;

    /**
     * @brief 获取 SkillRecipe. 若 SkillRecipe 存在, 则命中缓存并返回 SkillRecipe 数据; 若 SkillRecipe 不存在, 则对其进行初始化并返回 SkillRecipe 数据.
     * @param RecipeID
     * @return SkillRecipe&
     */
    static const SkillRecipe &get(int RecipeID, int RecipeLevel);
    static const Skill       *getSkill(const SkillRecipe *skillrecipe);

private:
    static inline std::mutex mutex; // 互斥锁. 用于保护 add 操作.

    struct tuple_hash {
        template <class T1, class T2>
        std::size_t operator()(const std::tuple<T1, T2> &t) const {
            auto h1 = std::hash<T1>{}(std::get<0>(t));
            auto h2 = std::hash<T2>{}(std::get<1>(t));
            return h1 ^ h2;
        }
    };

    /**
     * @brief SkillRecipe 缓存数据
     */
    static inline std::unordered_map<std::tuple<int, int>, SkillRecipe, tuple_hash> data;
    static inline std::unordered_map<const SkillRecipe *, Skill>                    ScriptSkill; // 用于保存 ScriptFile 作用的 Skill 对象

    /**
     * @brief 初始化 SkillRecipe. 将指定 ID 的 SkillRecipe 数据存至缓存.
     * @param RecipeID
     */
    static void add(int RecipeID, int RecipeLevel);
};

} // namespace ns_frame

#endif // FRAME_GLOBAL_SKILLRECIPE_H_
