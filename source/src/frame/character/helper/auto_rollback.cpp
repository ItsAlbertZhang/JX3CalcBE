#include "frame/character/helper/auto_rollback.h"
#include "frame/event.h"
#include "frame/runtime_lua.h"
#include "frame/static_ref.h"
#include "frame/static_refmap.h"
#include "program/log.h"

using namespace ns_frame;
using namespace ns_framestatic;

AutoRollbackTarget::AutoRollbackTarget(Character *self, Character *target) {
    this->self = self;
    this->target = self->target;
    self->target = target;
}

AutoRollbackTarget::~AutoRollbackTarget() {
    self->target = this->target;
}

AutoRollbackAttribute::AutoRollbackAttribute(Character *self, const Skill &skill, int atCriticalStrike, int atCriticalDamagePower, bool isCritical)
    : self(self), skill(&skill), atCriticalStrike(atCriticalStrike), atCriticalDamagePower(atCriticalDamagePower), isCritical(isCritical) {
    handle(false);
}
AutoRollbackAttribute::~AutoRollbackAttribute() {
    handle(true);
}

void AutoRollbackAttribute::handle(bool isRollback) {
    int c = isRollback ? -1 : 1;
    for (auto &it : skill->attrAttributes) {
        switch (it.mode) {

        case static_cast<int>(enumLuaAttributeEffectMode::EFFECT_TO_SELF_NOT_ROLLBACK): {
            if (isRollback)
                break;
            switch (it.type) {
            case static_cast<int>(enumLuaAttributeType::CAST_SKILL_TARGET_DST):
                self->chSkill.skillQueue.emplace(it.param1Int, it.param2);
                break;
            case static_cast<int>(enumLuaAttributeType::CAST_SKILL):
                self->chSkill.skillQueue.emplace(it.param1Int, it.param2);
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
                self->chDamage.damageList.emplace_back(
                    EventManager::now(),
                    skill->dwID, skill->dwLevel,
                    this->isCritical,
                    self->CalcDamage(
                        self->chAttr, self->target, DamageType::Physics,
                        atSolarDamage, atSolarDamageRand, atCriticalStrike, atCriticalDamagePower,
                        static_cast<int>(skill->nChannelInterval),
                        skill->nWeaponDamagePercent),
                    DamageType::Physics);
            } break;
            case static_cast<int>(enumLuaAttributeType::CALL_SOLAR_DAMAGE): {
                self->chDamage.damageList.emplace_back(
                    EventManager::now(),
                    skill->dwID, skill->dwLevel,
                    this->isCritical,
                    self->CalcDamage(
                        self->chAttr, self->target, DamageType::Solar,
                        atSolarDamage, atSolarDamageRand, atCriticalStrike, atCriticalDamagePower,
                        static_cast<int>(skill->nChannelInterval),
                        skill->nWeaponDamagePercent),
                    DamageType::Solar);
            } break;
            case static_cast<int>(enumLuaAttributeType::CALL_LUNAR_DAMAGE): {
                self->chDamage.damageList.emplace_back(
                    EventManager::now(),
                    skill->dwID, skill->dwLevel,
                    this->isCritical,
                    self->CalcDamage(
                        self->chAttr, self->target, DamageType::Lunar,
                        atSolarDamage, atSolarDamageRand, atCriticalStrike, atCriticalDamagePower,
                        static_cast<int>(skill->nChannelInterval),
                        skill->nWeaponDamagePercent),
                    DamageType::Lunar);
            } break;
            case static_cast<int>(enumLuaAttributeType::CALL_NEUTRAL_DAMAGE): {
                self->chDamage.damageList.emplace_back(
                    EventManager::now(),
                    skill->dwID, skill->dwLevel,
                    this->isCritical,
                    self->CalcDamage(
                        self->chAttr, self->target, DamageType::Neutral,
                        atSolarDamage, atSolarDamageRand, atCriticalStrike, atCriticalDamagePower,
                        static_cast<int>(skill->nChannelInterval),
                        skill->nWeaponDamagePercent),
                    DamageType::Neutral);
            } break;
            case static_cast<int>(enumLuaAttributeType::CALL_POISON_DAMAGE): {
                self->chDamage.damageList.emplace_back(
                    EventManager::now(),
                    skill->dwID, skill->dwLevel,
                    this->isCritical,
                    self->CalcDamage(
                        self->chAttr, self->target, DamageType::Poison,
                        atSolarDamage, atSolarDamageRand, atCriticalStrike, atCriticalDamagePower,
                        static_cast<int>(skill->nChannelInterval),
                        skill->nWeaponDamagePercent),
                    DamageType::Poison);
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