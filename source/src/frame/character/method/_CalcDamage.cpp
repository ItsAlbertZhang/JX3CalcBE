#include "frame/character/character.h"
#include "frame/global/skill.h"

using namespace ns_frame;

std::tuple<int, int> Character::CalcCritical(const CharacterAttr &attrSelf, int skillID, int skillLevel) {
    int atCriticalStrike = 0;
    int atCriticalDamagePower = 0;
    const Skill &skill = SkillManager::get(skillID, skillLevel);
    switch (skill.type) {
    case SkillType::Physics:
        atCriticalStrike = attrSelf.getPhysicsCriticalStrike();
        atCriticalDamagePower = attrSelf.getPhysicsCriticalDamagePower();
        break;
    case SkillType::SolarMagic:
        atCriticalStrike = attrSelf.getSolarCriticalStrike();
        atCriticalDamagePower = attrSelf.getSolarCriticalDamagePower();
        break;
    case SkillType::LunarMagic:
        atCriticalStrike = attrSelf.getLunarCriticalStrike();
        atCriticalDamagePower = attrSelf.getLunarCriticalDamagePower();
        break;
    case SkillType::NeutralMagic:
        atCriticalStrike = attrSelf.getNeutralCriticalStrike();
        atCriticalDamagePower = attrSelf.getNeutralCriticalDamagePower();
        break;
    case SkillType::Poison:
        atCriticalStrike = attrSelf.getPoisonCriticalStrike();
        atCriticalDamagePower = attrSelf.getPoisonCriticalDamagePower();
        break;
    }

    return std::make_tuple(atCriticalStrike, atCriticalDamagePower);
}

int Character::CalcDamage(const CharacterAttr &attrSelf, Character *target, DamageType typeDamage, int damageBase, int damageRand, int atCriticalStrike, int atCriticalDamagePower, int nChannelInterval, int nWeaponDamagePercent) {
    // TODO: 目标御劲降低会心率和会心效果.

    // 此处的快照并非指延时类的快照, 而是广义上的瞬间快照.
    // 例如, 在 Skill 的 AddAttribute 中提高的属性, 实际上会在一个快照中计算, 而非在角色属性中.
    // 因此, 应当尽量避免直接使用 this->chAttr, 而是使用 attrSelf, 除非明确知道这个属性不会被快照.
    // 若使用 this->chAttr, 则 Skill 的 AddAttribute 中提高的属性无法生效, 除非调用了 AdditionalAttribute(skill), 例如心法技能的 lua.

    int atStrain = this->chAttr.getStrain();                                // 类型× 快照
    int atDstNpcDamageCoefficient = this->chAttr.atDstNpcDamageCoefficient; // 类型× 快照
    int atAddDamageByDstMoveState = this->chAttr.atAddDamageByDstMoveState; // 类型× 快照

    int atAttackPower = 0;           // 类型√ 快照
    int atDamageAddPercent = 0;      // 类型√ 快照
    int atOvercome = 0;              // 类型√ 自身
    int targetShield = 0;            // 类型√ 目标
    int targetDamageCoefficient = 0; // 类型√ 目标
    int c = 12;
    int weaponDamage = 0;
    switch (typeDamage) {
    case DamageType::Physics:
        atAttackPower = attrSelf.getPhysicsAttackPower();
        atDamageAddPercent = attrSelf.getPhysicsDamageAddPercent();
        atOvercome = this->chAttr.getPhysicsOvercome();
        targetShield = target->chAttr.getPhysicsShield();
        targetDamageCoefficient = target->chAttr.atPhysicsDamageCoefficient;
        c = 10;
        weaponDamage = attrSelf.atMeleeWeaponDamageBase + attrSelf.atMeleeWeaponDamageRand / 2;
        weaponDamage = weaponDamage * nWeaponDamagePercent / 1024;
        break;
    case DamageType::Solar:
        atAttackPower = attrSelf.getSolarAttackPower();
        atDamageAddPercent = attrSelf.getSolarDamageAddPercent();
        atOvercome = this->chAttr.getSolarOvercome();
        targetShield = target->chAttr.getSolarShield();
        targetDamageCoefficient = target->chAttr.atSolarDamageCoefficient;
        break;
    case DamageType::Lunar:
        atAttackPower = attrSelf.getLunarAttackPower();
        atDamageAddPercent = attrSelf.getLunarDamageAddPercent();
        atOvercome = this->chAttr.getLunarOvercome();
        targetShield = target->chAttr.getLunarShield();
        targetDamageCoefficient = target->chAttr.atLunarDamageCoefficient;
        break;
    case DamageType::Neutral:
        atAttackPower = attrSelf.getNeutralAttackPower();
        atDamageAddPercent = attrSelf.getNeutralDamageAddPercent();
        atOvercome = this->chAttr.getNeutralOvercome();
        targetShield = target->chAttr.getNeutralShield();
        targetDamageCoefficient = target->chAttr.atNeutralDamageCoefficient;
        break;
    case DamageType::Poison:
        atAttackPower = attrSelf.getPoisonAttackPower();
        atDamageAddPercent = attrSelf.getPoisonDamageAddPercent();
        atOvercome = this->chAttr.getPoisonOvercome();
        targetShield = target->chAttr.getPoisonShield();
        targetDamageCoefficient = target->chAttr.atPoisonDamageCoefficient;
        break;
    }

    int damage = damageBase + damageRand / 2;
    damage = damage + atAttackPower * nChannelInterval / c / 16 + weaponDamage;
    if (!target->isPlayer) {
        damage = damage * (1024 + atStrain) / 1024;
        damage = damage * (1024 + atDstNpcDamageCoefficient) / 1024;
    }
    if (true) {
        // TODO: 实现目标移动状态
        damage = damage * (1024 + atAddDamageByDstMoveState) / 1024;
    }
    damage = damage * (1024 + atDamageAddPercent) / 1024;
    damage = damage * (1024 + atOvercome) / 1024;
    damage = damage * (1024 - targetShield) / 1024;
    damage = damage * (1024 + targetDamageCoefficient) / 1024;

    return damage;
}