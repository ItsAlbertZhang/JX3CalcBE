#include "frame/character/helper/auto_rollback_attribute.h"
#include "frame/event.h"
#include "frame/runtime_lua.h"   // LuaFunc
#include "frame/static_ref.h"    // enumLuaAttributeEffectMode
#include "frame/static_refmap.h" // enumLuaAttributeType
#include "program/log.h"
#include <random>

using namespace ns_frame;
using namespace ns_framestatic;

AutoRollbackAttribute::AutoRollbackAttribute(Character *self, Character *target, RuntimeCastSkill *runtime, const Skill &skill)
    : self(self), target(target), runtime(runtime), skill(skill) {
    handle(false);
}
AutoRollbackAttribute::~AutoRollbackAttribute() {
    handle(true);
}

bool AutoRollbackAttribute::CallDamage(int DamageAddPercent) {
    // 计算会心
    auto [atCriticalStrike, atCriticalDamagePower] = self->CalcCritical(self->chAttr, skill.dwID, skill.dwLevel);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 9999);
    bool isCritical = dis(gen) < atCriticalStrike;
    // 计算伤害
    for (int i = 0; i < static_cast<int>(DamageType::COUNT); i++) {
        if (callDamage[i]) {
            runtime->damageList.emplace_back(
                Event::now(),
                skill.dwID, skill.dwLevel,
                isCritical,
                self->CalcDamage(
                    self->chAttr, target, static_cast<DamageType>(i),
                    isCritical, atCriticalDamagePower, DamageAddPercent,
                    atDamage[i], atDamageRand[i],
                    static_cast<int>(skill.nChannelInterval),
                    skill.nWeaponDamagePercent),
                static_cast<DamageType>(i));
        }
        if (callSurplusDamage[i]) {
            runtime->damageList.emplace_back(
                Event::now(),
                skill.dwID, skill.dwLevel,
                isCritical,
                self->CalcDamage(
                    self->chAttr, target, static_cast<DamageType>(i),
                    isCritical, atCriticalDamagePower, DamageAddPercent,
                    0, 0,
                    this->atGlobalDamageFactor, 0,
                    1, 1, true),
                static_cast<DamageType>(i));
        }
    }
    return isCritical;
}

void AutoRollbackAttribute::handle(bool isRollback) {
    int c = isRollback ? -1 : 1;
    for (auto &it : skill.attrAttributes) {
        switch (it.mode) {

        case static_cast<int>(enumLuaAttributeEffectMode::EFFECT_TO_SELF_NOT_ROLLBACK): {
            if (isRollback) // NOT_ROLLBACK
                break;
            switch (it.type) {
            case static_cast<int>(enumLuaAttributeType::CAST_SKILL_TARGET_DST):
                runtime->skillQueue.emplace(it.param1Int, it.param2, self, target);
                break;
            case static_cast<int>(enumLuaAttributeType::CAST_SKILL):
                runtime->skillQueue.emplace(it.param1Int, it.param2, self);
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
                this->atDamage[static_cast<int>(DamageType::Physics)] += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SKILL_PHYSICS_DAMAGE_RAND):
                this->atDamageRand[static_cast<int>(DamageType::Physics)] += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SKILL_SOLAR_DAMAGE):
                this->atDamage[static_cast<int>(DamageType::Solar)] += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SKILL_SOLAR_DAMAGE_RAND):
                this->atDamageRand[static_cast<int>(DamageType::Solar)] += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SKILL_LUNAR_DAMAGE):
                this->atDamage[static_cast<int>(DamageType::Lunar)] += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SKILL_LUNAR_DAMAGE_RAND):
                this->atDamageRand[static_cast<int>(DamageType::Lunar)] += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SKILL_NEUTRAL_DAMAGE):
                this->atDamage[static_cast<int>(DamageType::Neutral)] += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SKILL_NEUTRAL_DAMAGE_RAND):
                this->atDamageRand[static_cast<int>(DamageType::Neutral)] += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SKILL_POISON_DAMAGE):
                this->atDamage[static_cast<int>(DamageType::Poison)] += it.param1Int * c;
                break;
            case static_cast<int>(enumLuaAttributeType::SKILL_POISON_DAMAGE_RAND):
                this->atDamageRand[static_cast<int>(DamageType::Poison)] += it.param1Int * c;
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
            if (target == nullptr) // TO_DEST
                break;
            if (isRollback) // NOT_ROLLBACK
                break;
            switch (it.type) {
            case static_cast<int>(enumLuaAttributeType::EXECUTE_SCRIPT): {
                std::string paramStr = "scripts/" + it.param1Str;
                int dwCharacterID = Character::getCharacterID(target);
                int dwSkillSrcID = Character::getCharacterID(self);
                LuaFunc::analysis(LuaFunc::getApply(paramStr)(dwCharacterID, dwSkillSrcID), paramStr, LuaFunc::Enum::Apply);
            } break;
            case static_cast<int>(enumLuaAttributeType::EXECUTE_SCRIPT_WITH_PARAM): {
                std::string paramStr = "scripts/" + it.param1Str;
                int dwCharacterID = Character::getCharacterID(target);
                int dwSkillSrcID = Character::getCharacterID(self);
                LuaFunc::analysis(LuaFunc::getApply(paramStr)(dwCharacterID, it.param2, dwSkillSrcID), paramStr, LuaFunc::Enum::Apply);
            } break;
            case static_cast<int>(enumLuaAttributeType::CALL_PHYSICS_DAMAGE):
                this->callDamage[static_cast<int>(DamageType::Physics)] = true;
                break;
            case static_cast<int>(enumLuaAttributeType::CALL_SOLAR_DAMAGE):
                this->callDamage[static_cast<int>(DamageType::Solar)] = true;
                break;
            case static_cast<int>(enumLuaAttributeType::CALL_LUNAR_DAMAGE):
                this->callDamage[static_cast<int>(DamageType::Lunar)] = true;
                break;
            case static_cast<int>(enumLuaAttributeType::CALL_NEUTRAL_DAMAGE):
                this->callDamage[static_cast<int>(DamageType::Neutral)] = true;
                break;
            case static_cast<int>(enumLuaAttributeType::CALL_POISON_DAMAGE):
                this->callDamage[static_cast<int>(DamageType::Poison)] = true;
                break;
            case static_cast<int>(enumLuaAttributeType::CALL_SURPLUS_PHYSICS_DAMAGE):
                this->callSurplusDamage[static_cast<int>(DamageType::Physics)] = true;
                break;
            case static_cast<int>(enumLuaAttributeType::CALL_SURPLUS_SOLAR_DAMAGE):
                this->callSurplusDamage[static_cast<int>(DamageType::Solar)] = true;
                break;
            case static_cast<int>(enumLuaAttributeType::CALL_SURPLUS_LUNAR_DAMAGE):
                this->callSurplusDamage[static_cast<int>(DamageType::Lunar)] = true;
                break;
            case static_cast<int>(enumLuaAttributeType::CALL_SURPLUS_NEUTRAL_DAMAGE):
                this->callSurplusDamage[static_cast<int>(DamageType::Neutral)] = true;
                break;
            case static_cast<int>(enumLuaAttributeType::CALL_SURPLUS_POISON_DAMAGE):
                this->callSurplusDamage[static_cast<int>(DamageType::Poison)] = true;
                break;
            default:
                LOG_ERROR("Undefined: %s, %s: %d %d, rollback=%d\n", refLuaAttributeEffectMode[it.mode], refLuaAttributeType[it.type], it.param1Int, it.param2, isRollback);
            }
        } break; // EFFECT_TO_DEST_NOT_ROLLBACK

        case static_cast<int>(enumLuaAttributeEffectMode::EFFECT_TO_DEST_AND_ROLLBACK): {
            if (target == nullptr) // TO_DEST
                break;
            switch (it.type) {
            case static_cast<int>(enumLuaAttributeType::GLOBAL_DAMGAGE_FACTOR):
                this->atGlobalDamageFactor += it.param1Int * c;
                break;
            default:
                LOG_ERROR("Undefined: %s, %s: %d %d, rollback=%d\n", refLuaAttributeEffectMode[it.mode], refLuaAttributeType[it.type], it.param1Int, it.param2, isRollback);
            }
        } break; // EFFECT_TO_DEST_AND_ROLLBACK

        } // switch (it.mode)
    }
}