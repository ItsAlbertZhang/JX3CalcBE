#include "frame/character/helper/auto_rollback_attribute.h"
#include "frame/event.h"
#include "frame/runtime_lua.h"   // LuaFunc
#include "frame/static_ref.h"    // enumLuaAttributeEffectMode
#include "frame/static_refmap.h" // enumLuaAttributeType
#include "program/log.h"

using namespace ns_frame;
using namespace ns_framestatic;

AutoRollbackAttribute::AutoRollbackAttribute(Character *self, const Skill &skill, int atCriticalStrike, int atCriticalDamagePower, int DamageAddPercent, SkillRuntime *runtime)
    : self(self), skill(skill), atCriticalStrike(atCriticalStrike), atCriticalDamagePower(atCriticalDamagePower), DamageAddPercent(DamageAddPercent), runtime(runtime) {
    handle(false);
}
AutoRollbackAttribute::~AutoRollbackAttribute() {
    handle(true);
}

void AutoRollbackAttribute::handle(bool isRollback) {
    int c = isRollback ? -1 : 1;
    for (auto &it : skill.attrAttributes) {
        switch (it.mode) {

        case static_cast<int>(enumLuaAttributeEffectMode::EFFECT_TO_SELF_NOT_ROLLBACK): {
            if (isRollback)
                break;
            switch (it.type) {
            case static_cast<int>(enumLuaAttributeType::CAST_SKILL_TARGET_DST):
                runtime->skillQueue.emplace(it.param1Int, it.param2);
                break;
            case static_cast<int>(enumLuaAttributeType::CAST_SKILL):
                runtime->skillQueue.emplace(it.param1Int, it.param2);
                break;
            case static_cast<int>(enumLuaAttributeType::EXECUTE_SCRIPT): {
                std::string paramStr = "scripts/" + it.param1Str;
                int dwCharacterID = Character::getCharacterID(self);
                int dwSkillSrcID = Character::getCharacterID(self);
                LuaFunc::analysis(LuaFunc::getApply(paramStr)(dwCharacterID, dwSkillSrcID), paramStr, LuaFunc::Enum::Apply);
            } break;
            case static_cast<int>(enumLuaAttributeType::EXECUTE_SCRIPT_WITH_PARAM): {
                std::string paramStr = "scripts/" + it.param1Str;
                int dwCharacterID = Character::getCharacterID(self);
                int dwSkillSrcID = Character::getCharacterID(self);
                LuaFunc::analysis(LuaFunc::getApply(paramStr)(dwCharacterID, it.param2, dwSkillSrcID), paramStr, LuaFunc::Enum::Apply);
            } break;
            case static_cast<int>(enumLuaAttributeType::CURRENT_SUN_ENERGY):
                self->nCurrentSunEnergy += it.param1Int;
                break;
            case static_cast<int>(enumLuaAttributeType::CURRENT_MOON_ENERGY):
                self->nCurrentMoonEnergy += it.param1Int;
                break;
            default:
                LOG_ERROR("Undefined: %s, %s: %d %d, rollback=%d\n", refLuaAttributeEffectMode[it.mode], refLuaAttributeType[it.type], it.param1Int, it.param2, isRollback);
            }
        } break; // EFFECT_TO_SELF_NOT_ROLLBACK

        case static_cast<int>(enumLuaAttributeEffectMode::EFFECT_TO_SELF_AND_ROLLBACK): {
            switch (it.type) {
            case static_cast<int>(enumLuaAttributeType::SKILL_PHYSICS_DAMAGE):
                this->atPhysicsDamage += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SKILL_PHYSICS_DAMAGE_RAND):
                this->atPhysicsDamageRand += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SKILL_SOLAR_DAMAGE):
                this->atSolarDamage += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SKILL_SOLAR_DAMAGE_RAND):
                this->atSolarDamageRand += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SKILL_LUNAR_DAMAGE):
                this->atLunarDamage += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SKILL_LUNAR_DAMAGE_RAND):
                this->atLunarDamageRand += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SKILL_NEUTRAL_DAMAGE):
                this->atNeutralDamage += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SKILL_NEUTRAL_DAMAGE_RAND):
                this->atNeutralDamageRand += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SKILL_POISON_DAMAGE):
                this->atPoisonDamage += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SKILL_POISON_DAMAGE_RAND):
                this->atPoisonDamageRand += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::PHYSICS_ATTACK_POWER_PERCENT):
                self->chAttr.atPhysicsAttackPowerPercent += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::EXECUTE_SCRIPT): {
                std::string paramStr = "scripts/" + it.param1Str;
                int dwCharacterID = Character::getCharacterID(self);
                int dwSkillSrcID = Character::getCharacterID(self);
                LuaFunc::analysis(LuaFunc::getApply(paramStr)(dwCharacterID, dwSkillSrcID), paramStr, LuaFunc::Enum::Apply);
            } break;
            case static_cast<int>(enumLuaAttributeType::EXECUTE_SCRIPT_WITH_PARAM): {
                std::string paramStr = "scripts/" + it.param1Str;
                int dwCharacterID = Character::getCharacterID(self);
                int dwSkillSrcID = Character::getCharacterID(self);
                LuaFunc::analysis(LuaFunc::getApply(paramStr)(dwCharacterID, it.param2, dwSkillSrcID), paramStr, LuaFunc::Enum::Apply);
            } break;
            case static_cast<int>(enumLuaAttributeType::DST_NPC_DAMAGE_COEFFICIENT):
                self->chAttr.atDstNpcDamageCoefficient += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::MAGIC_SHIELD):
                self->chAttr.atMagicShield += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::PHYSICS_SHIELD_BASE):
                self->chAttr.atPhysicsShieldBase += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::DROP_DEFENCE):
                // 未做相关实现
                break;
            case static_cast<int>(enumLuaAttributeType::ACTIVE_THREAT_COEFFICIENT):
                // 未做相关实现
                break;
            case static_cast<int>(enumLuaAttributeType::BEAT_BACK_RATE):
                // 未做相关实现
                break;
            case static_cast<int>(enumLuaAttributeType::SPUNK_TO_SOLAR_AND_LUNAR_ATTACK_POWER_COF):
                self->chAttr.atSpunkToSolarAndLunarAttackPowerCof += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SPUNK_TO_SOLAR_AND_LUNAR_CRITICAL_STRIKE_COF):
                self->chAttr.atSpunkToSolarAndLunarCriticalStrikeCof += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::MAX_SUN_ENERGY):
                self->chAttr.atMaxSunEnergy += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::MAX_MOON_ENERGY):
                self->chAttr.atMaxMoonEnergy += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::MAX_LIFE_PERCENT_ADD):
                // 未做相关实现
                break;
            case static_cast<int>(enumLuaAttributeType::SOLAR_ATTACK_POWER_BASE):
                self->chAttr.atSolarAttackPowerBase += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::LUNAR_ATTACK_POWER_BASE):
                self->chAttr.atLunarAttackPowerBase += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::MANA_REPLENISH_PERCENT):
                // 未做相关实现
                break;
            case static_cast<int>(enumLuaAttributeType::DECRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE):
                self->chAttr.atDecriticalDamagePowerBaseKiloNumRate += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SOLAR_CRITICAL_STRIKE_BASE_RATE):
                self->chAttr.atSolarCriticalStrikeBaseRate += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::LUNAR_CRITICAL_STRIKE_BASE_RATE):
                self->chAttr.atLunarCriticalStrikeBaseRate += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::MAGIC_CRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE):
                self->chAttr.atMagicCriticalDamagePowerBaseKiloNumRate += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SET_ADAPTIVE_SKILL_TYPE):
                if (isRollback)
                    self->atAdaptiveSkillType = enumLuaSkillKindType::COUNT;
                else
                    self->atAdaptiveSkillType = static_cast<enumLuaSkillKindType>(it.param1Int);
                break;
            case static_cast<int>(enumLuaAttributeType::SET_TALENT_RECIPE):
                if (isRollback)
                    self->chSkillRecipe.remove(it.param1Int, it.param2);
                else
                    self->chSkillRecipe.add(it.param1Int, it.param2);
                break;
            case static_cast<int>(enumLuaAttributeType::SKILL_EVENT_HANDLER):
                if (isRollback)
                    self->chSkillEvent.remove(it.param1Int);
                else
                    self->chSkillEvent.add(it.param1Int);
                break;
            default:
                LOG_ERROR("Undefined: %s, %s: %d %d, rollback=%d\n", refLuaAttributeEffectMode[it.mode], refLuaAttributeType[it.type], it.param1Int, it.param2, isRollback);
            }
        } break; // EFFECT_TO_SELF_AND_ROLLBACK

        case static_cast<int>(enumLuaAttributeEffectMode::EFFECT_TO_DEST_NOT_ROLLBACK): {
            if (isRollback)
                break;
            switch (it.type) {
            case static_cast<int>(enumLuaAttributeType::EXECUTE_SCRIPT): {
                std::string paramStr = "scripts/" + it.param1Str;
                int dwCharacterID = Character::getCharacterID(self->target);
                int dwSkillSrcID = Character::getCharacterID(self);
                LuaFunc::analysis(LuaFunc::getApply(paramStr)(dwCharacterID, dwSkillSrcID), paramStr, LuaFunc::Enum::Apply);
            } break;
            case static_cast<int>(enumLuaAttributeType::EXECUTE_SCRIPT_WITH_PARAM): {
                std::string paramStr = "scripts/" + it.param1Str;
                int dwCharacterID = Character::getCharacterID(self->target);
                int dwSkillSrcID = Character::getCharacterID(self);
                LuaFunc::analysis(LuaFunc::getApply(paramStr)(dwCharacterID, it.param2, dwSkillSrcID), paramStr, LuaFunc::Enum::Apply);
            } break;
            case static_cast<int>(enumLuaAttributeType::CALL_PHYSICS_DAMAGE): {
                runtime->dmgPhysics += self->CalcDamage(
                    self->chAttr, self->target, DamageType::Physics,
                    runtime->isCritical, atCriticalDamagePower, DamageAddPercent,
                    atPhysicsDamage, atPhysicsDamageRand,
                    static_cast<int>(skill.nChannelInterval),
                    skill.nWeaponDamagePercent);
            } break;
            case static_cast<int>(enumLuaAttributeType::CALL_SOLAR_DAMAGE): {
                runtime->dmgSolar += self->CalcDamage(
                    self->chAttr, self->target, DamageType::Solar,
                    runtime->isCritical, atCriticalDamagePower, DamageAddPercent,
                    atSolarDamage, atSolarDamageRand,
                    static_cast<int>(skill.nChannelInterval),
                    skill.nWeaponDamagePercent);
            } break;
            case static_cast<int>(enumLuaAttributeType::CALL_LUNAR_DAMAGE): {
                runtime->dmgLunar += self->CalcDamage(
                    self->chAttr, self->target, DamageType::Lunar,
                    runtime->isCritical, atCriticalDamagePower, DamageAddPercent,
                    atLunarDamage, atLunarDamageRand,
                    static_cast<int>(skill.nChannelInterval),
                    skill.nWeaponDamagePercent);
            } break;
            case static_cast<int>(enumLuaAttributeType::CALL_NEUTRAL_DAMAGE): {
                runtime->dmgNeutral += self->CalcDamage(
                    self->chAttr, self->target, DamageType::Neutral,
                    runtime->isCritical, atCriticalDamagePower, DamageAddPercent,
                    atNeutralDamage, atNeutralDamageRand,
                    static_cast<int>(skill.nChannelInterval),
                    skill.nWeaponDamagePercent);
            } break;
            case static_cast<int>(enumLuaAttributeType::CALL_POISON_DAMAGE): {
                runtime->dmgPoison += self->CalcDamage(
                    self->chAttr, self->target, DamageType::Poison,
                    runtime->isCritical, atCriticalDamagePower, DamageAddPercent,
                    atPoisonDamage, atPoisonDamageRand,
                    static_cast<int>(skill.nChannelInterval),
                    skill.nWeaponDamagePercent);
            } break;
            default:
                LOG_ERROR("Undefined: %s, %s: %d %d, rollback=%d\n", refLuaAttributeEffectMode[it.mode], refLuaAttributeType[it.type], it.param1Int, it.param2, isRollback);
            }
        } break; // EFFECT_TO_DEST_NOT_ROLLBACK

        case static_cast<int>(enumLuaAttributeEffectMode::EFFECT_TO_DEST_AND_ROLLBACK): {
            // switch (it.type) {
            // default:
            LOG_ERROR("Undefined: %s, %s: %d %d, rollback=%d\n", refLuaAttributeEffectMode[it.mode], refLuaAttributeType[it.type], it.param1Int, it.param2, isRollback);
            // }
        } break; // EFFECT_TO_DEST_AND_ROLLBACK

        } // switch (it.mode)
    }
}