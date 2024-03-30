#include "frame/character/character.h"
#include "frame/common/damage.h"
#include "frame/event.h"
#include "frame/global/skill.h"
#include "plugin/channelinterval.h"
#include "plugin/log.h"

using namespace jx3calc;
using namespace frame;

std::tuple<int, int> Character::calcCritical(const ChAttr &attrSelf, int skillID, int skillLevel) {
    // TODO: 目标御劲降低会心率和会心效果.
    int          atCriticalStrike      = 0;
    int          atCriticalDamagePower = 0;
    const Skill &skill                 = SkillManager::get(skillID, skillLevel);
    if (!skill.HasCriticalStrike)
        return std::make_tuple(atCriticalStrike, atCriticalDamagePower);

    switch (skill.KindType) {
    case ref::Skill::KindType::Physics:
        atCriticalStrike      = attrSelf.getPhysicsCriticalStrike();
        atCriticalDamagePower = attrSelf.getPhysicsCriticalDamagePower();
        break;
    case ref::Skill::KindType::SolarMagic:
        atCriticalStrike      = attrSelf.getSolarCriticalStrike();
        atCriticalDamagePower = attrSelf.getSolarCriticalDamagePower();
        break;
    case ref::Skill::KindType::LunarMagic:
        atCriticalStrike      = attrSelf.getLunarCriticalStrike();
        atCriticalDamagePower = attrSelf.getLunarCriticalDamagePower();
        break;
    case ref::Skill::KindType::NeutralMagic:
        atCriticalStrike      = attrSelf.getNeutralCriticalStrike();
        atCriticalDamagePower = attrSelf.getNeutralCriticalDamagePower();
        break;
    case ref::Skill::KindType::Poison:
        atCriticalStrike      = attrSelf.getPoisonCriticalStrike();
        atCriticalDamagePower = attrSelf.getPoisonCriticalDamagePower();
        break;
    default:
        CONSTEXPR_LOG_ERROR("Unknown skill KindType: {}", static_cast<int>(skill.KindType));
        break;
    }

    atCriticalStrike = atCriticalStrike > 10000 ? 10000 : atCriticalStrike;

    return std::make_tuple(atCriticalStrike, atCriticalDamagePower);
}

Damage Character::calcDamage(
    int              id,
    int              level,
    const ChAttr    &attrSelf,
    const Character *target,
    DamageType       typeDamage,
    int              atCriticalStrike,
    int              atCriticalDamagePower,
    int              damageBase,
    int              damageRand,
    int              damageAddPercent,
    int              nChannelInterval,
    int              nWeaponDamagePercent,
    bool             isCritical,
    bool             isSurplus,
    bool             isBuff,
    int              buffInterval,
    int              buffCount
) {
    int atStrain                  = this->chAttr.getStrain();               // 类型× 快照
    int atSurplus                 = this->chAttr.getSurplus();              // 类型× 快照
    int atDstNpcDamageCoefficient = this->chAttr.atDstNpcDamageCoefficient; // 类型× 快照
    int atGlobalDamageFactor      = this->chAttr.atGlobalDamageFactor;      // 类型× 快照
    int atAddDamageByDstMoveState = this->chAttr.atAddDamageByDstMoveState; // 类型× 快照

    int atAttackPower           = 0; // 类型√ 快照
    int atDamageAddPercent      = 0; // 类型√ 快照
    int atOvercome              = 0; // 类型√ 自身实时
    int targetShield            = 0; // 类型√ 目标实时
    int targetDamageCoefficient = 0; // 类型√ 目标实时

    int levelCof = 100 - (target->chAttr.atLevel - attrSelf.atLevel) * 5; // 等级压制, 注意仅对 NPC 有效

    int c            = 12;
    int weaponDamage = 0;

    int coeffCount    = isBuff ? buffCount : 1;
    int coeffInterval = isBuff ? buffInterval * buffCount / 12 : 1;
    coeffInterval     = coeffInterval > 16 ? coeffInterval : 16;

    switch (typeDamage) {
    case DamageType::Physics:
        atAttackPower           = attrSelf.getPhysicsAttackPower();
        atDamageAddPercent      = attrSelf.getPhysicsDamageAddPercent();
        atOvercome              = this->chAttr.getPhysicsOvercome();
        targetShield            = target->chAttr.getPhysicsShield(this->chAttr.atAllShieldIgnorePercent);
        targetDamageCoefficient = target->chAttr.atPhysicsDamageCoefficient;
        c                       = 10;
        weaponDamage            = attrSelf.atMeleeWeaponDamageBase + attrSelf.atMeleeWeaponDamageRand / 2;
        weaponDamage            = weaponDamage * nWeaponDamagePercent / 1024;
        break;
    case DamageType::Solar:
        atAttackPower           = attrSelf.getSolarAttackPower();
        atDamageAddPercent      = attrSelf.getSolarDamageAddPercent();
        atOvercome              = this->chAttr.getSolarOvercome();
        targetShield            = target->chAttr.getSolarShield(this->chAttr.atAllShieldIgnorePercent);
        targetDamageCoefficient = target->chAttr.atSolarDamageCoefficient;
        break;
    case DamageType::Lunar:
        atAttackPower           = attrSelf.getLunarAttackPower();
        atDamageAddPercent      = attrSelf.getLunarDamageAddPercent();
        atOvercome              = this->chAttr.getLunarOvercome();
        targetShield            = target->chAttr.getLunarShield(this->chAttr.atAllShieldIgnorePercent);
        targetDamageCoefficient = target->chAttr.atLunarDamageCoefficient;
        break;
    case DamageType::Neutral:
        atAttackPower           = attrSelf.getNeutralAttackPower();
        atDamageAddPercent      = attrSelf.getNeutralDamageAddPercent();
        atOvercome              = this->chAttr.getNeutralOvercome();
        targetShield            = target->chAttr.getNeutralShield(this->chAttr.atAllShieldIgnorePercent);
        targetDamageCoefficient = target->chAttr.atNeutralDamageCoefficient;
        break;
    case DamageType::Poison:
        atAttackPower           = attrSelf.getPoisonAttackPower();
        atDamageAddPercent      = attrSelf.getPoisonDamageAddPercent();
        atOvercome              = this->chAttr.getPoisonOvercome();
        targetShield            = target->chAttr.getPoisonShield(this->chAttr.atAllShieldIgnorePercent);
        targetDamageCoefficient = target->chAttr.atPoisonDamageCoefficient;
        break;
    default:
        CONSTEXPR_LOG_ERROR("Unknown damage type: {}", static_cast<int>(typeDamage));
        break;
    }

    unsigned long long damage = damageBase + damageRand / 2;
    if (isSurplus) {
        CONSTEXPR_CHANNELINTERVAL_RECORD(
            id,
            level,
            damageBase,
            damageRand,
            static_cast<double>(1) * (atGlobalDamageFactor + (1 << 20)) / (1 << 20),
            isBuff
        );
        damage = damage + atSurplus * 1;
    } else {
        CONSTEXPR_CHANNELINTERVAL_RECORD(
            id,
            level,
            damageBase,
            damageRand,
            static_cast<double>(1) * nChannelInterval * coeffInterval / 16 / coeffCount / c / 16 * (atGlobalDamageFactor + (1 << 20)) / (1 << 20),
            isBuff
        );
        damage = damage + atAttackPower * nChannelInterval * coeffInterval / 16 / coeffCount / c / 16 + weaponDamage;
    }
    damage = damage * (atGlobalDamageFactor + (1 << 20)) / (1 << 20);
    if (!target->isPlayer) {
        damage = damage * (1024 + atStrain) / 1024;
        damage = damage * (1024 + atDstNpcDamageCoefficient) / 1024;
        damage = damage * levelCof / 100;
    }
    damage = damage * (1024 + atOvercome) / 1024;
    damage = damage * (1024 + atDamageAddPercent + damageAddPercent) / 1024;
    if (true) {
        // TODO: 实现目标移动状态
        damage = damage * (1024 + atAddDamageByDstMoveState) / 1024;
    }
    damage = damage * (1024 - targetShield) / 1024;
    damage = damage * (1024 + targetDamageCoefficient) / 1024;

    unsigned long long damageCritical = damage * (1792 + atCriticalDamagePower) / 1024;
    unsigned long long damageExcept   = (damage * (10000 - atCriticalStrike) + damageCritical * atCriticalStrike) / 10000;

    return Damage{
        .tick           = Event::now(),
        .damageType     = typeDamage,
        .id             = id,
        .level          = level,
        .damageBase     = static_cast<int>(damage),
        .damageCritical = static_cast<int>(damageCritical),
        .damageExcept   = static_cast<int>(damageExcept),
        .criticalRate   = atCriticalStrike,
        .isCritical     = isCritical,
        .isBuff         = isBuff,
    };
}
