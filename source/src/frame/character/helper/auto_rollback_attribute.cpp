#include "frame/character/helper/auto_rollback_attribute.h"
#include "frame/character/character.h"
#include "frame/character/helper/runtime_castskill.h"
#include "frame/common/damage.h"
#include "frame/lua/interface.h"          // LuaFunc
#include "frame/ref/lua_attribute_type.h" // ref::lua::ATTRIBUTE_TYPE
#include "frame/ref/lua_normal.h"         // ref::lua::ATTRIBUTE_EFFECT_MODE
#include "plugin/log.h"
#include <random>

using namespace jx3calc;
using namespace frame;

AutoRollbackAttribute::AutoRollbackAttribute(Character *self, Character *target, RuntimeCastSkill *runtime, const Skill &skill) :
    self(self), target(target), runtime(runtime), skill(skill) {
    handle(false);
}
AutoRollbackAttribute::~AutoRollbackAttribute() {
    handle(true);
}

bool AutoRollbackAttribute::CallDamage(int DamageAddPercent) {
    // 计算会心
    auto [atCriticalStrike, atCriticalDamagePower] = self->calcCritical(self->chAttr, skill.dwSkillID, skill.dwLevel);
    std::random_device              rd;
    std::mt19937                    gen(rd());
    std::uniform_int_distribution<> dis(0, 9999);
    bool                            isCritical = dis(gen) < atCriticalStrike;
    // 计算伤害
    Damage                          item;
    for (int idxType = 0; idxType < static_cast<int>(DamageType::COUNT); idxType++) {
        for (int idxTime = 0; idxTime < callDamage[idxType]; idxTime++) {
            item += self->calcDamage(
                skill.dwSkillID,
                skill.dwLevel,
                self->chAttr,
                target,
                static_cast<DamageType>(idxType),
                atCriticalStrike,
                atCriticalDamagePower,
                atDamage[idxType],
                atDamageRand[idxType],
                DamageAddPercent,
                static_cast<int>(skill.nChannelInterval),
                skill.nWeaponDamagePercent,
                isCritical,
                false,
                false,
                1,
                1
            );
        }
        for (int idxTime = 0; idxTime < callSurplusDamage[idxType]; idxTime++) {
            item += self->calcDamage(
                skill.dwSkillID,
                skill.dwLevel,
                self->chAttr,
                target,
                static_cast<DamageType>(idxType),
                atCriticalStrike,
                atCriticalDamagePower,
                0,
                0,
                DamageAddPercent,
                1, // 破招伤害的调整是通过 atGlobalDamageFactor 实现的, 这符合游戏内的实际原理
                0,
                isCritical,
                true,
                false,
                1,
                1
            );
        }
    }
    if (item.damageType)
        runtime->damageList.emplace_back(std::move(item));
    return isCritical;
}

void AutoRollbackAttribute::handle(bool isRollback) {
    int c = isRollback ? -1 : 1;
    for (auto &it : skill.attrAttributes) {
        switch (it.mode) {

        case static_cast<int>(ref::lua::ATTRIBUTE_EFFECT_MODE::EFFECT_TO_SELF_NOT_ROLLBACK): {
            if (isRollback) // NOT_ROLLBACK
                break;
            switch (it.type) {
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CAST_SKILL):                runtime->skillQueue.emplace(it); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CAST_SKILL_TARGET_DST):     runtime->skillQueue.emplace(it); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::EXECUTE_SCRIPT):            runtime->skillQueue.emplace(it); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::EXECUTE_SCRIPT_WITH_PARAM): runtime->skillQueue.emplace(it); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CURRENT_SUN_ENERGY):        self->nCurrentSunEnergy += it.param1Int; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CURRENT_MOON_ENERGY):       self->nCurrentMoonEnergy += it.param1Int; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SUN_POWER_VALUE):           self->nSunPowerValue = it.param1Int; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::MOON_POWER_VALUE):          self->nMoonPowerValue = it.param1Int; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_BUFF):
                self->buffAdd(self->dwID, self->nLevel, it.param1Int, it.param2);
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::DEL_SINGLE_BUFF_BY_ID_AND_LEVEL):
                self->buffDel(it.param1Int, it.param2);
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::DEL_MULTI_GROUP_BUFF_BY_FUNCTIONTYPE):
                break; // 未做相关实现, 目前其仅在解控时使用
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::DEL_MULTI_GROUP_BUFF_BY_ID):
                self->buffDelMultiGroupByID(it.param1Int);
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::STOP):      break; // 未做相关实现, 推测为停止, 用于解除击飞
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::DO_ACTION): break; // 未做相关实现, 推测为动作
            default:
                CONSTEXPR_LOG_ERROR("Undefined: {}, {}: {} {}, rollback={}", Ref<ref::lua::ATTRIBUTE_EFFECT_MODE>::names[it.mode], Ref<ref::lua::ATTRIBUTE_TYPE>::names[it.type], it.param1Int, it.param2, isRollback);
                break;
            }
        } break; // EFFECT_TO_SELF_NOT_ROLLBACK

        case static_cast<int>(ref::lua::ATTRIBUTE_EFFECT_MODE::EFFECT_TO_SELF_AND_ROLLBACK): {
            switch (it.type) {
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::DROP_DEFENCE):                                  break; // 未做相关实现, 推测为摔落保护
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::ACTIVE_THREAT_COEFFICIENT):                     break; // 未做相关实现, 推测为威胁值
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::BEAT_BACK_RATE):                                break; // 未做相关实现, 推测为运功被打退概率
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::MAX_LIFE_PERCENT_ADD):                          break; // 未做相关实现, 额外最大生命值
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::MANA_REPLENISH_PERCENT):                        break; // 未做相关实现, 推测为内力回复
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::KUNGFU_TYPE):                                   break; // 未做相关实现, 推测为武学类型, 用于心法 lua 中转换全能属性
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_PHYSICS_DAMAGE):                          this->atDamage[static_cast<int>(DamageType::Physics)] += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_PHYSICS_DAMAGE_RAND):                     this->atDamageRand[static_cast<int>(DamageType::Physics)] += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_SOLAR_DAMAGE):                            this->atDamage[static_cast<int>(DamageType::Solar)] += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_SOLAR_DAMAGE_RAND):                       this->atDamageRand[static_cast<int>(DamageType::Solar)] += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_NEUTRAL_DAMAGE):                          this->atDamage[static_cast<int>(DamageType::Neutral)] += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_NEUTRAL_DAMAGE_RAND):                     this->atDamageRand[static_cast<int>(DamageType::Neutral)] += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_LUNAR_DAMAGE):                            this->atDamage[static_cast<int>(DamageType::Lunar)] += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_LUNAR_DAMAGE_RAND):                       this->atDamageRand[static_cast<int>(DamageType::Lunar)] += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_POISON_DAMAGE):                           this->atDamage[static_cast<int>(DamageType::Poison)] += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_POISON_DAMAGE_RAND):                      this->atDamageRand[static_cast<int>(DamageType::Poison)] += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::ALL_DAMAGE_ADD_PERCENT):                        self->chAttr.atAllDamageAddPercent += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::DST_NPC_DAMAGE_COEFFICIENT):                    self->chAttr.atDstNpcDamageCoefficient += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::MAGIC_SHIELD):                                  self->chAttr.atMagicShield += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::PHYSICS_SHIELD_BASE):                           self->chAttr.atPhysicsShieldBase += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SPUNK_TO_SOLAR_AND_LUNAR_ATTACK_POWER_COF):     self->chAttr.atSpunkToSolarAndLunarAttackPowerCof += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SPUNK_TO_SOLAR_AND_LUNAR_CRITICAL_STRIKE_COF):  self->chAttr.atSpunkToSolarAndLunarCriticalStrikeCof += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::MAX_SUN_ENERGY):                                self->chAttr.atMaxSunEnergy += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::MAX_MOON_ENERGY):                               self->chAttr.atMaxMoonEnergy += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SOLAR_ATTACK_POWER_BASE):                       self->chAttr.atSolarAttackPowerBase += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::LUNAR_ATTACK_POWER_BASE):                       self->chAttr.atLunarAttackPowerBase += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::PHYSICS_ATTACK_POWER_PERCENT):                  self->chAttr.atPhysicsAttackPowerPercent += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SOLAR_ATTACK_POWER_PERCENT):                    self->chAttr.atSolarAttackPowerPercent += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::LUNAR_ATTACK_POWER_PERCENT):                    self->chAttr.atLunarAttackPowerPercent += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::DECRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE):     self->chAttr.atDecriticalDamagePowerBaseKiloNumRate += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::PHYSICS_CRITICAL_STRIKE_BASE_RATE):             self->chAttr.atPhysicsCriticalStrikeBaseRate += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SOLAR_CRITICAL_STRIKE_BASE_RATE):               self->chAttr.atSolarCriticalStrikeBaseRate += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::NEUTRAL_CRITICAL_STRIKE_BASE_RATE):             self->chAttr.atNeutralCriticalStrikeBaseRate += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::LUNAR_CRITICAL_STRIKE_BASE_RATE):               self->chAttr.atLunarCriticalStrikeBaseRate += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::POISON_CRITICAL_STRIKE_BASE_RATE):              self->chAttr.atPoisonCriticalStrikeBaseRate += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::MAGIC_CRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE): self->chAttr.atMagicCriticalDamagePowerBaseKiloNumRate += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SOLAR_CRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE): self->chAttr.atSolarCriticalDamagePowerBaseKiloNumRate += it.param1Int * c; break;

            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::EXECUTE_SCRIPT):                                {
                if (!isRollback) {
                    std::string paramStr      = "scripts/" + it.param1Str;
                    int         dwCharacterID = Character::characterGetID(self);
                    int         dwSkillSrcID  = Character::characterGetID(self);
                    if (!lua::interface::analysis(lua::interface::getApply(paramStr)(dwCharacterID, dwSkillSrcID), paramStr, lua::interface::FuncType::Apply))
                        CONSTEXPR_LOG_ERROR("LuaFunc::getApply(\"{}\") failed.", paramStr);
                }
            } break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::EXECUTE_SCRIPT_WITH_PARAM): {
                if (!isRollback) {
                    std::string paramStr      = "scripts/" + it.param1Str;
                    int         dwCharacterID = Character::characterGetID(self);
                    int         dwSkillSrcID  = Character::characterGetID(self);
                    if (!lua::interface::analysis(lua::interface::getApply(paramStr)(dwCharacterID, it.param2, dwSkillSrcID), paramStr, lua::interface::FuncType::Apply))
                        CONSTEXPR_LOG_ERROR("LuaFunc::getApply(\"{}\") failed.", paramStr);
                }
            } break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SET_ADAPTIVE_SKILL_TYPE):
                self->atAdaptiveSkillType = static_cast<ref::lua::SKILL_KIND_TYPE>(it.param1Int);
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SET_TALENT_RECIPE):
                if (isRollback)
                    self->skillrecipeRemove(it.param1Int, it.param2);
                else
                    self->skillrecipeAdd(it.param1Int, it.param2);
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_EVENT_HANDLER):
                if (isRollback)
                    self->skilleventRemove(it.param1Int);
                else
                    self->skilleventAdd(it.param1Int);
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::ADD_DAMAGE_BY_DST_MOVE_STATE):
                // param1Int 为目标移动状态. 暂时不对其进行处理, 统一直接使用 param2 进行增伤.
                self->chAttr.atAddDamageByDstMoveState += it.param2 * c;
                break;
            default:
                CONSTEXPR_LOG_ERROR("Undefined: {}, {}: {} {}, rollback={}", Ref<ref::lua::ATTRIBUTE_EFFECT_MODE>::names[it.mode], Ref<ref::lua::ATTRIBUTE_TYPE>::names[it.type], it.param1Int, it.param2, isRollback);
                break;
            }
        } break; // EFFECT_TO_SELF_AND_ROLLBACK

        case static_cast<int>(ref::lua::ATTRIBUTE_EFFECT_MODE::EFFECT_TO_DEST_NOT_ROLLBACK): {
            if (target == nullptr) // TO_DEST
                break;
            if (isRollback) // NOT_ROLLBACK
                break;
            switch (it.type) {
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_PHYSICS_DAMAGE):
                this->callDamage[static_cast<int>(DamageType::Physics)] += 1;
                this->self->bFightState = true;
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_SOLAR_DAMAGE):
                this->callDamage[static_cast<int>(DamageType::Solar)] += 1;
                this->self->bFightState = true;
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_NEUTRAL_DAMAGE):
                this->callDamage[static_cast<int>(DamageType::Neutral)] += 1;
                this->self->bFightState = true;
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_LUNAR_DAMAGE):
                this->callDamage[static_cast<int>(DamageType::Lunar)] += 1;
                this->self->bFightState = true;
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_POISON_DAMAGE):
                this->callDamage[static_cast<int>(DamageType::Poison)] += 1;
                this->self->bFightState = true;
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_ADAPTIVE_DAMAGE): {
                switch (self->atAdaptiveSkillType) {
                case ref::lua::SKILL_KIND_TYPE::PHYSICS:
                    this->callDamage[static_cast<int>(DamageType::Physics)] += 1;
                    break;
                case ref::lua::SKILL_KIND_TYPE::SOLAR_MAGIC:
                    this->callDamage[static_cast<int>(DamageType::Solar)] += 1;
                    break;
                case ref::lua::SKILL_KIND_TYPE::NEUTRAL_MAGIC:
                    this->callDamage[static_cast<int>(DamageType::Neutral)] += 1;
                    break;
                case ref::lua::SKILL_KIND_TYPE::LUNAR_MAGIC:
                    this->callDamage[static_cast<int>(DamageType::Lunar)] += 1;
                    break;
                case ref::lua::SKILL_KIND_TYPE::POISON_MAGIC:
                    this->callDamage[static_cast<int>(DamageType::Poison)] += 1;
                    break;
                default:
                    break;
                }
            } break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_SURPLUS_PHYSICS_DAMAGE):
                this->callSurplusDamage[static_cast<int>(DamageType::Physics)] += 1;
                this->self->bFightState = true;
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_SURPLUS_SOLAR_DAMAGE):
                this->callSurplusDamage[static_cast<int>(DamageType::Solar)] += 1;
                this->self->bFightState = true;
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_SURPLUS_NEUTRAL_DAMAGE):
                this->callSurplusDamage[static_cast<int>(DamageType::Neutral)] += 1;
                this->self->bFightState = true;
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_SURPLUS_LUNAR_DAMAGE):
                this->callSurplusDamage[static_cast<int>(DamageType::Lunar)] += 1;
                this->self->bFightState = true;
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_SURPLUS_POISON_DAMAGE):
                this->callSurplusDamage[static_cast<int>(DamageType::Poison)] += 1;
                this->self->bFightState = true;
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::EXECUTE_SCRIPT): {
                std::string paramStr      = "scripts/" + it.param1Str;
                int         dwCharacterID = Character::characterGetID(target);
                int         dwSkillSrcID  = Character::characterGetID(self);
                if (!lua::interface::analysis(lua::interface::getApply(paramStr)(dwCharacterID, dwSkillSrcID), paramStr, lua::interface::FuncType::Apply))
                    CONSTEXPR_LOG_ERROR("LuaFunc::getApply(\"{}\") failed.", paramStr);
            } break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::EXECUTE_SCRIPT_WITH_PARAM): {
                std::string paramStr      = "scripts/" + it.param1Str;
                int         dwCharacterID = Character::characterGetID(target);
                int         dwSkillSrcID  = Character::characterGetID(self);
                if (!lua::interface::analysis(lua::interface::getApply(paramStr)(dwCharacterID, it.param2, dwSkillSrcID), paramStr, lua::interface::FuncType::Apply))
                    CONSTEXPR_LOG_ERROR("LuaFunc::getApply(\"{}\") failed.", paramStr);
            } break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_BUFF):
                target->buffAdd(self->dwID, self->nLevel, it.param1Int, it.param2);
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::DASH):
                // 未做相关实现, 推测为冲刺
                break;
            default:
                CONSTEXPR_LOG_ERROR("Undefined: {}, {}: {} {}, rollback={}", Ref<ref::lua::ATTRIBUTE_EFFECT_MODE>::names[it.mode], Ref<ref::lua::ATTRIBUTE_TYPE>::names[it.type], it.param1Int, it.param2, isRollback);
                break;
            }
        } break; // EFFECT_TO_DEST_NOT_ROLLBACK

        case static_cast<int>(ref::lua::ATTRIBUTE_EFFECT_MODE::EFFECT_TO_DEST_AND_ROLLBACK): {
            if (target == nullptr) // TO_DEST
                break;
            switch (it.type) {
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::GLOBAL_DAMGAGE_FACTOR):
                this->self->chAttr.atGlobalDamageFactor += it.param1Int * c;
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::LUNAR_MAGIC_SHIELD_PERCENT):
                this->self->chAttr.atLunarMagicShieldPercent += it.param1Int * c;
                break;
            default:
                CONSTEXPR_LOG_ERROR("Undefined: {}, {}: {} {}, rollback={}", Ref<ref::lua::ATTRIBUTE_EFFECT_MODE>::names[it.mode], Ref<ref::lua::ATTRIBUTE_TYPE>::names[it.type], it.param1Int, it.param2, isRollback);
                break;
            }
        } break; // EFFECT_TO_DEST_AND_ROLLBACK

        default:
            break;

        } // switch (it.mode)
    }
}
