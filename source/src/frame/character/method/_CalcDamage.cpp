#include "frame/character/character.h"
#include "frame/global/skill.h"
#include "program/log.h"

using namespace ns_frame;

std::tuple<int, int> Character::CalcCritical(const CharacterAttr &attrSelf, int skillID, int skillLevel) {
    // TODO: 目标御劲降低会心率和会心效果.
    int atCriticalStrike = 0;
    int atCriticalDamagePower = 0;
    const Skill &skill = SkillManager::get(skillID, skillLevel);
    if (!skill.HasCriticalStrike)
        return std::make_tuple(atCriticalStrike, atCriticalDamagePower);

    switch (skill.KindType) {
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
    default:
        LOG_ERROR("Unknown skill KindType: %d", static_cast<int>(skill.KindType));
        break;
    }

    return std::make_tuple(atCriticalStrike, atCriticalDamagePower);
}

int Character::CalcDamage(const CharacterAttr &attrSelf, Character *target, DamageType typeDamage, bool isCritical, int atCriticalDamagePower, int DamageAddPercent, int damageBase, int damageRand, int nChannelInterval, int nWeaponDamagePercent) {
    int atStrain = this->chAttr.getStrain();                                // 类型× 快照
    int atDstNpcDamageCoefficient = this->chAttr.atDstNpcDamageCoefficient; // 类型× 快照
    int atAddDamageByDstMoveState = this->chAttr.atAddDamageByDstMoveState; // 类型× 快照

    int atAttackPower = 0;           // 类型√ 快照
    int atDamageAddPercent = 0;      // 类型√ 快照
    int atOvercome = 0;              // 类型√ 自身实时
    int targetShield = 0;            // 类型√ 目标实时
    int targetDamageCoefficient = 0; // 类型√ 目标实时
    int c = 12;
    int weaponDamage = 0;
    switch (typeDamage) {
    case DamageType::Physics:
        atAttackPower = attrSelf.getPhysicsAttackPower();
        atDamageAddPercent = attrSelf.getPhysicsDamageAddPercent();
        atOvercome = this->chAttr.getPhysicsOvercome();
        targetShield = target->chAttr.getPhysicsShield(attrSelf.atAllShieldIgnorePercent);
        targetDamageCoefficient = target->chAttr.atPhysicsDamageCoefficient;
        c = 10;
        weaponDamage = attrSelf.atMeleeWeaponDamageBase + attrSelf.atMeleeWeaponDamageRand / 2;
        weaponDamage = weaponDamage * nWeaponDamagePercent / 1024;
        break;
    case DamageType::Solar:
        atAttackPower = attrSelf.getSolarAttackPower();
        atDamageAddPercent = attrSelf.getSolarDamageAddPercent();
        atOvercome = this->chAttr.getSolarOvercome();
        targetShield = target->chAttr.getSolarShield(attrSelf.atAllShieldIgnorePercent);
        targetDamageCoefficient = target->chAttr.atSolarDamageCoefficient;
        break;
    case DamageType::Lunar:
        atAttackPower = attrSelf.getLunarAttackPower();
        atDamageAddPercent = attrSelf.getLunarDamageAddPercent();
        atOvercome = this->chAttr.getLunarOvercome();
        targetShield = target->chAttr.getLunarShield(attrSelf.atAllShieldIgnorePercent);
        targetDamageCoefficient = target->chAttr.atLunarDamageCoefficient;
        break;
    case DamageType::Neutral:
        atAttackPower = attrSelf.getNeutralAttackPower();
        atDamageAddPercent = attrSelf.getNeutralDamageAddPercent();
        atOvercome = this->chAttr.getNeutralOvercome();
        targetShield = target->chAttr.getNeutralShield(attrSelf.atAllShieldIgnorePercent);
        targetDamageCoefficient = target->chAttr.atNeutralDamageCoefficient;
        break;
    case DamageType::Poison:
        atAttackPower = attrSelf.getPoisonAttackPower();
        atDamageAddPercent = attrSelf.getPoisonDamageAddPercent();
        atOvercome = this->chAttr.getPoisonOvercome();
        targetShield = target->chAttr.getPoisonShield(attrSelf.atAllShieldIgnorePercent);
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
    damage = damage * (1024 + atDamageAddPercent + DamageAddPercent) / 1024;
    damage = damage * (1024 + atOvercome) / 1024;
    damage = damage * (1024 - targetShield) / 1024;
    damage = damage * (1024 + targetDamageCoefficient) / 1024;

    if (isCritical) {
        damage = damage * (1792 + atCriticalDamagePower) / 1024;
    }

    return damage;
}