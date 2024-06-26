#pragma once

#include "frame/common/damage.h"
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
    AutoRollbackAttribute(Character *self, Character *target, RuntimeCastSkill *runtime, const Skill &skill);
    AutoRollbackAttribute(const AutoRollbackAttribute &)            = delete;
    AutoRollbackAttribute &operator=(const AutoRollbackAttribute &) = delete;
    AutoRollbackAttribute(AutoRollbackAttribute &&)                 = delete;
    AutoRollbackAttribute &operator=(AutoRollbackAttribute &&)      = delete;
    ~AutoRollbackAttribute();

    bool CallDamage(int DamageAddPercent);

private:
    Character        *self;
    Character        *target;
    RuntimeCastSkill *runtime;
    const Skill      &skill;

    int atDamage[static_cast<int>(DamageType::COUNT)]          = {0};
    int atDamageRand[static_cast<int>(DamageType::COUNT)]      = {0};
    int callDamage[static_cast<int>(DamageType::COUNT)]        = {0};
    int callSurplusDamage[static_cast<int>(DamageType::COUNT)] = {0};

    void handle(bool isRollback);
};

} // namespace frame
} // namespace jx3calc
