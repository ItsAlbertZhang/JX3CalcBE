#pragma once

#include "frame/common/damage.h"
#include "frame/global/skill.h"
#include <queue>

namespace jx3calc {
namespace frame {

class Character;

/**
 * @brief 自动回滚的魔法属性
 * @param self 自身
 * @param skill 技能
 * @note 当 AutoRollbackAttribute 对象销毁时, 会自动将被 ROLLBACK 影响的属性回滚到原来的值.
 */
class AutoRollbackAttribute {
public:
    AutoRollbackAttribute(
        Character                        *self,
        Character                        *target,
        AutoRollbackAttribute            *ancestor,
        const Skill                      &skill,
        const std::vector<const Skill *> *skillrecipeList,
        int                               skillID,
        int                               skillLevel,
        int                               damageAddPercent
    );
    AutoRollbackAttribute(const AutoRollbackAttribute &)            = delete;
    AutoRollbackAttribute &operator=(const AutoRollbackAttribute &) = delete;
    AutoRollbackAttribute(AutoRollbackAttribute &&)                 = delete;
    AutoRollbackAttribute &operator=(AutoRollbackAttribute &&)      = delete;
    ~AutoRollbackAttribute();

    bool                  getCritical() const;
    std::tuple<int, int> &emplace(int skillID, int skillLevel);

    auto proxyRecipe(auto &&func, auto &&...args) -> decltype(func((args)...)) {
        loadRecipe();
        if constexpr (std::is_void_v<decltype(func((args)...))>) {
            func((args)...);
            unloadRecipe();
        } else {
            auto res = func((args)...);
            unloadRecipe();
            return res;
        }
    }

private:
    Character                        *self;
    Character                        *target;
    AutoRollbackAttribute            *ancestor;
    const Skill                      &skill;
    const std::vector<const Skill *> *skillrecipeList;
    int                               skillID;
    int                               skillLevel;
    int                               damageAddPercent;
    int                               criticalStrike;
    int                               criticalDamagePower;
    bool                              isCritical;

    int atPhysicsDamage     = 0;
    int atPhysicsDamageRand = 0;
    int atSolarDamage       = 0;
    int atSolarDamageRand   = 0;
    int atNeutralDamage     = 0;
    int atNeutralDamageRand = 0;
    int atLunarDamage       = 0;
    int atLunarDamageRand   = 0;
    int atPoisonDamage      = 0;
    int atPoisonDamageRand  = 0;

    std::queue<std::tuple<int, int>> skillQueue;
    Damage                           damage;

    std::vector<std::unique_ptr<AutoRollbackAttribute>> recipeSkills;

    void handle(bool isRollback);
    void loadRecipe();
    void unloadRecipe();
};

} // namespace frame
} // namespace jx3calc
