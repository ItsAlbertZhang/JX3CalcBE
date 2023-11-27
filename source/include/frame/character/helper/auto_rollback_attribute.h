#ifndef FRAME_CHARACTER_HELPER_AUTO_ROLLBACK_ATTRIBUTE_H_
#define FRAME_CHARACTER_HELPER_AUTO_ROLLBACK_ATTRIBUTE_H_

#include "frame/character/character.h"
#include "frame/global/skill.h"

namespace ns_frame {

/**
 * @brief 自动回滚的魔法属性
 * @param self 自身
 * @param skill 技能
 * @note 当 AutoRollbackAttribute 对象销毁时, 会自动将被 ROLLBACK 影响的属性回滚到原来的值.
 */
class AutoRollbackAttribute {
public:
    AutoRollbackAttribute(Character *self, const Skill &skill, int atCriticalStrike, int atCriticalDamagePower, bool isCritical);
    ~AutoRollbackAttribute();

private:
    Character *self;
    const Skill &skill;
    const int atCriticalStrike = 0;
    const int atCriticalDamagePower = 0;
    const bool isCritical = false;
    int atPhysicsDamage = 0;
    int atPhysicsDamageRand = 0;
    int atSolarDamage = 0;
    int atSolarDamageRand = 0;
    int atLunarDamage = 0;
    int atLunarDamageRand = 0;
    int atNeutralDamage = 0;
    int atNeutralDamageRand = 0;
    int atPoisonDamage = 0;
    int atPoisonDamageRand = 0;

    void handle(bool isRollback);
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_HELPER_AUTO_ROLLBACK_ATTRIBUTE_H_