#pragma once

#include "frame/global/skill.h"

namespace jx3calc {
namespace frame {

class Character;
class RuntimeCastSkill;

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
        RuntimeCastSkill                 *runtime,
        const Skill                      &skill,
        const std::vector<const Skill *> *skillrecipeList,
        int                               damageAddPercent,
        int                               criticalStrike,
        int                               criticalDamagePower,
        bool                              isCritical
    );
    AutoRollbackAttribute(const AutoRollbackAttribute &)            = delete;
    AutoRollbackAttribute &operator=(const AutoRollbackAttribute &) = delete;
    AutoRollbackAttribute(AutoRollbackAttribute &&)                 = delete;
    AutoRollbackAttribute &operator=(AutoRollbackAttribute &&)      = delete;
    ~AutoRollbackAttribute();

private:
    Character                        *self;
    Character                        *target;
    RuntimeCastSkill                 *runtime;
    const Skill                      &skill;
    const std::vector<const Skill *> *skillrecipeList;
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

    void handle(bool isRollback);
};

} // namespace frame
} // namespace jx3calc
