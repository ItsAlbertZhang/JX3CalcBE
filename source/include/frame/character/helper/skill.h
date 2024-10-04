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
class HelperSkill {
public:
    HelperSkill(
        Character                        *self,
        Character                        *target,
        HelperSkill                      *ancestor,
        const Skill                      &skill,
        const std::vector<const Skill *> *recipeSkills,
        int                               damageAddPercent
    );
    HelperSkill(const HelperSkill &)            = delete;
    HelperSkill &operator=(const HelperSkill &) = delete;
    HelperSkill(HelperSkill &&)                 = delete;
    HelperSkill &operator=(HelperSkill &&)      = delete;
    ~HelperSkill();

    bool                  getCritical() const;
    std::tuple<int, int> &emplace(int skillID, int skillLevel);

    auto proxyRecipe(auto &&func, auto &&...args) -> decltype(func((args)...)) {
        recipeLoad();
        if constexpr (std::is_void_v<decltype(func((args)...))>) {
            func((args)...);
            recipeUnload();
        } else {
            auto res = func((args)...);
            recipeUnload();
            return res;
        }
    }

private:
    Character                        *self;
    Character                        *target;
    HelperSkill                      *ancestor;
    const Skill                      &skill;
    const std::vector<const Skill *> *recipeSkills;
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

    Damage                                    damage;
    std::queue<std::tuple<int, int>>          skillQueue;
    std::vector<std::unique_ptr<HelperSkill>> recipesActive;

    void handle(bool isRollback);
    void recipeLoad();
    void recipeUnload();
};

} // namespace frame
} // namespace jx3calc
