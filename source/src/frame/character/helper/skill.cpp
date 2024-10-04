#include "frame/character/helper/skill.h"
#include "frame/character/character.h"
#include "frame/common/damage.h"
#include "frame/lua/interface.h"          // LuaFunc
#include "frame/ref/lua_attribute_type.h" // ref::lua::ATTRIBUTE_TYPE
#include "frame/ref/lua_normal.h"         // ref::lua::ATTRIBUTE_EFFECT_MODE
#include "plugin/log.h"
#include <optional>
#include <random>
// #include <queue>
// #include <variant>

using namespace jx3calc;
using namespace frame;

HelperSkill::HelperSkill(
    Character *self, Character *target, HelperSkill *ancestor, const Skill &skill,
    const std::vector<const Skill *> *recipeSkills, int damageAddPercent
) :
    self(self), target(target), ancestor(ancestor ? ancestor : this), skill(skill),
    recipeSkills(recipeSkills), damageAddPercent(damageAddPercent) // constructor
{
    const auto calcCritical = [](HelperSkill *self) {
        std::tuple<int, int> res  = self->self->calcCritical(self->self->chAttr, self->skill.dwSkillID, self->skill.dwLevel);
        self->criticalStrike      = std::get<0>(res);
        self->criticalDamagePower = std::get<1>(res);
        std::random_device              rd;
        std::mt19937                    gen(rd());
        std::uniform_int_distribution<> dis(0, 9999);
        self->isCritical = dis(gen) < self->criticalStrike;
    };
    // TODO: #29 目前根据是否是技能(而非秘籍)来判断是否需要计算会心. 技能的 ancestor 为 this, 秘籍的 ancestor 则为技能.
    // 但是, 在大多数情况下, 并不需要真正计算会心. 因此后续考虑将会心计算的条件更改为: 是否存在 Critical 的 SkillEvent.
    if (this->ancestor == this)
        proxyRecipe(calcCritical, this);
    handle(false);
}

HelperSkill::~HelperSkill() {
    handle(true);
    while (!skillQueue.empty()) {
        auto it = std::move(skillQueue.front());
        skillQueue.pop();
        self->skillCast(self->targetCurr, std::get<0>(it), std::get<1>(it));
    }
    if (damage.id != 0) {
        self->chDamage.emplace_back(std::move(damage));
    }
}

bool HelperSkill::getCritical() const {
    return this->isCritical;
}

std::tuple<int, int> &HelperSkill::emplace(int skillID, int skillLevel) {
    return skillQueue.emplace(std::make_tuple(skillID, skillLevel));
}

void HelperSkill::recipeLoad() {
    if (recipeSkills == nullptr)
        return;
    this->recipesActive.clear();
    this->recipesActive.reserve(recipeSkills->size());
    for (const auto &it : *recipeSkills) {
        this->recipesActive.emplace_back(std::make_unique<HelperSkill>(self, target, ancestor, *it, nullptr, 0));
    }
}

void HelperSkill::recipeUnload() {
    this->recipesActive.clear();
}

void HelperSkill::handle(bool isRollback) {
    const auto callDamage = [](HelperSkill *self, bool isDest, DamageType type, int damageBase, int damageRand, bool isSurplus) -> void {
        if (isDest && self->target == nullptr) [[unlikely]]
            return;
        Character *target = isDest ? self->target : self->self;

        self->self->bFightState = true;
        self->ancestor->damage += self->self->calcDamage(
            self->skill.dwSkillID,
            self->skill.dwLevel,
            self->self->chAttr,
            target,
            type,
            self->criticalStrike,
            self->criticalDamagePower,
            damageBase,
            damageRand,
            self->damageAddPercent,
            isSurplus ? 1 : static_cast<int>(self->skill.nChannelInterval),
            isSurplus ? 0 : self->skill.nWeaponDamagePercent,
            self->isCritical,
            isSurplus,
            false,
            1,
            1,
            self->skill.IsFrost
        );
    };
    const auto executeScript = [](HelperSkill *self, bool isDest, const std::string &param1Str, std::optional<int> param2, bool isRollback) -> void {
        if (isDest && self->target == nullptr) [[unlikely]]
            return;
        auto target        = isDest ? self->target : self->self;
        auto filename      = "scripts/" + param1Str;
        auto luaFunc       = isRollback ? lua::interface::getUnApply : lua::interface::getApply;
        auto dwCharacterID = Character::characterGetID(target);
        auto dwSkillSrcID  = Character::characterGetID(self->self);
        auto res =
            param2.has_value()
                ? luaFunc(filename)(dwCharacterID, param2.value(), dwSkillSrcID)
                : luaFunc(filename)(dwCharacterID, dwSkillSrcID);
        if (!lua::interface::analysis(std::move(res), filename, lua::interface::FuncType::Apply))
            CONSTEXPR_LOG_ERROR("LuaFunc::getApply(\"{}\") failed.", filename);
    };

#define INT(x) static_cast<int>(x)
    using Mode = ref::lua::ATTRIBUTE_EFFECT_MODE;
    using Type = ref::lua::ATTRIBUTE_TYPE;

    int c = isRollback ? -1 : 1;
    for (auto &it : skill.attrAttributes) {
        switch (it.mode) {
        case INT(Mode::EFFECT_TO_SELF_AND_ROLLBACK): {
            switch (it.type) {
            case INT(Type::DROP_DEFENCE):                                  break; // 未做相关实现, 推测为摔落保护
            case INT(Type::ACTIVE_THREAT_COEFFICIENT):                     break; // 未做相关实现, 推测为威胁值
            case INT(Type::BEAT_BACK_RATE):                                break; // 未做相关实现, 推测为运功被打退概率
            case INT(Type::MAX_LIFE_PERCENT_ADD):                          break; // 未做相关实现, 额外最大生命值
            case INT(Type::MANA_REPLENISH_PERCENT):                        break; // 未做相关实现, 推测为内力回复
            case INT(Type::KUNGFU_TYPE):                                   break; // 未做相关实现, 推测为武学类型, 用于心法 lua 中转换全能属性
            case INT(Type::SKILL_PHYSICS_DAMAGE):                          this->atPhysicsDamage += it.param1Int * c; break;
            case INT(Type::SKILL_PHYSICS_DAMAGE_RAND):                     this->atPhysicsDamageRand += it.param1Int * c; break;
            case INT(Type::SKILL_SOLAR_DAMAGE):                            this->atSolarDamage += it.param1Int * c; break;
            case INT(Type::SKILL_SOLAR_DAMAGE_RAND):                       this->atSolarDamageRand += it.param1Int * c; break;
            case INT(Type::SKILL_NEUTRAL_DAMAGE):                          this->atNeutralDamage += it.param1Int * c; break;
            case INT(Type::SKILL_NEUTRAL_DAMAGE_RAND):                     this->atNeutralDamageRand += it.param1Int * c; break;
            case INT(Type::SKILL_LUNAR_DAMAGE):                            this->atLunarDamage += it.param1Int * c; break;
            case INT(Type::SKILL_LUNAR_DAMAGE_RAND):                       this->atLunarDamageRand += it.param1Int * c; break;
            case INT(Type::SKILL_POISON_DAMAGE):                           this->atPoisonDamage += it.param1Int * c; break;
            case INT(Type::SKILL_POISON_DAMAGE_RAND):                      this->atPoisonDamageRand += it.param1Int * c; break;
            case INT(Type::ALL_DAMAGE_ADD_PERCENT):                        self->chAttr.atAllDamageAddPercent += it.param1Int * c; break;
            case INT(Type::DST_NPC_DAMAGE_COEFFICIENT):                    self->chAttr.atDstNpcDamageCoefficient += it.param1Int * c; break;
            case INT(Type::MAGIC_SHIELD):                                  self->chAttr.atMagicShield += it.param1Int * c; break;
            case INT(Type::PHYSICS_SHIELD_BASE):                           self->chAttr.atPhysicsShieldBase += it.param1Int * c; break;
            case INT(Type::SPUNK_TO_SOLAR_AND_LUNAR_ATTACK_POWER_COF):     self->chAttr.atSpunkToSolarAndLunarAttackPowerCof += it.param1Int * c; break;
            case INT(Type::SPUNK_TO_SOLAR_AND_LUNAR_CRITICAL_STRIKE_COF):  self->chAttr.atSpunkToSolarAndLunarCriticalStrikeCof += it.param1Int * c; break;
            case INT(Type::MAX_SUN_ENERGY):                                self->chAttr.atMaxSunEnergy += it.param1Int * c; break;
            case INT(Type::MAX_MOON_ENERGY):                               self->chAttr.atMaxMoonEnergy += it.param1Int * c; break;
            case INT(Type::SOLAR_ATTACK_POWER_BASE):                       self->chAttr.atSolarAttackPowerBase += it.param1Int * c; break;
            case INT(Type::LUNAR_ATTACK_POWER_BASE):                       self->chAttr.atLunarAttackPowerBase += it.param1Int * c; break;
            case INT(Type::PHYSICS_ATTACK_POWER_PERCENT):                  self->chAttr.atPhysicsAttackPowerPercent += it.param1Int * c; break;
            case INT(Type::SOLAR_ATTACK_POWER_PERCENT):                    self->chAttr.atSolarAttackPowerPercent += it.param1Int * c; break;
            case INT(Type::LUNAR_ATTACK_POWER_PERCENT):                    self->chAttr.atLunarAttackPowerPercent += it.param1Int * c; break;
            case INT(Type::DECRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE):     self->chAttr.atDecriticalDamagePowerBaseKiloNumRate += it.param1Int * c; break;
            case INT(Type::PHYSICS_CRITICAL_STRIKE_BASE_RATE):             self->chAttr.atPhysicsCriticalStrikeBaseRate += it.param1Int * c; break;
            case INT(Type::SOLAR_CRITICAL_STRIKE_BASE_RATE):               self->chAttr.atSolarCriticalStrikeBaseRate += it.param1Int * c; break;
            case INT(Type::NEUTRAL_CRITICAL_STRIKE_BASE_RATE):             self->chAttr.atNeutralCriticalStrikeBaseRate += it.param1Int * c; break;
            case INT(Type::LUNAR_CRITICAL_STRIKE_BASE_RATE):               self->chAttr.atLunarCriticalStrikeBaseRate += it.param1Int * c; break;
            case INT(Type::POISON_CRITICAL_STRIKE_BASE_RATE):              self->chAttr.atPoisonCriticalStrikeBaseRate += it.param1Int * c; break;
            case INT(Type::MAGIC_CRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE): self->chAttr.atMagicCriticalDamagePowerBaseKiloNumRate += it.param1Int * c; break;
            case INT(Type::SOLAR_CRITICAL_DAMAGE_POWER_BASE_KILONUM_RATE): self->chAttr.atSolarCriticalDamagePowerBaseKiloNumRate += it.param1Int * c; break;
            case INT(Type::EXECUTE_SCRIPT):                                executeScript(this, false, it.param1Str, std::nullopt, isRollback); break;
            case INT(Type::EXECUTE_SCRIPT_WITH_PARAM):                     executeScript(this, false, it.param1Str, it.param2, isRollback); break;
            case INT(Type::SET_ADAPTIVE_SKILL_TYPE):
                if (isRollback)
                    self->atAdaptiveSkillType = static_cast<ref::lua::SKILL_KIND_TYPE>(it.param1Int);
                else
                    self->atAdaptiveSkillType = ref::lua::SKILL_KIND_TYPE::COUNT;
                break;
            case INT(Type::SET_TALENT_RECIPE):
                if (isRollback)
                    self->skillrecipeRemove(it.param1Int, it.param2);
                else
                    self->skillrecipeAdd(it.param1Int, it.param2);
                break;
            case INT(Type::SKILL_EVENT_HANDLER):
                if (isRollback)
                    self->skilleventRemove(it.param1Int);
                else
                    self->skilleventAdd(it.param1Int);
                break;
            case INT(Type::ADD_DAMAGE_BY_DST_MOVE_STATE):
                // param1Int 为目标移动状态. 暂时不对其进行处理, 统一直接使用 param2 进行增伤.
                self->chAttr.atAddDamageByDstMoveState += it.param2 * c;
                break;
            default:
                CONSTEXPR_LOG_ERROR("Undefined: {}, {}: {} {}, rollback={}", Ref<Mode>::names[it.mode], Ref<Type>::names[it.type], it.param1Int, it.param2, isRollback);
                break;
            }
        } break; // EFFECT_TO_SELF_AND_ROLLBACK
        case INT(Mode::EFFECT_TO_DEST_AND_ROLLBACK): {
            if (target == nullptr) // TO_DEST
                break;
            switch (it.type) {
            case INT(Type::GLOBAL_DAMGAGE_FACTOR): target->chAttr.atGlobalDamageFactor += it.param1Int * c; break;
            default:
                CONSTEXPR_LOG_ERROR("Undefined: {}, {}: {} {}, rollback={}", Ref<Mode>::names[it.mode], Ref<Type>::names[it.type], it.param1Int, it.param2, isRollback);
                break;
            }
        } break; // EFFECT_TO_DEST_AND_ROLLBACK

        default:
            break;

        } // switch (it.mode)
    } // for (auto &it : skill.attrAttributes)

    for (auto &it : skill.attrAttributes) {
        switch (it.mode) {

        case INT(Mode::EFFECT_TO_SELF_NOT_ROLLBACK): {
            if (isRollback) // NOT_ROLLBACK
                break;
            switch (it.type) {
            case INT(Type::CAST_SKILL):                           this->ancestor->skillQueue.emplace(std::make_tuple(it.param1Int, it.param2)); break;
            case INT(Type::CAST_SKILL_TARGET_DST):                this->ancestor->skillQueue.emplace(std::make_tuple(it.param1Int, it.param2)); break;
            case INT(Type::EXECUTE_SCRIPT):                       executeScript(this, false, it.param1Str, std::nullopt, false); break;
            case INT(Type::EXECUTE_SCRIPT_WITH_PARAM):            executeScript(this, false, it.param1Str, it.param2, false); break;
            case INT(Type::CURRENT_SUN_ENERGY):                   self->nCurrentSunEnergy += it.param1Int; break;
            case INT(Type::CURRENT_MOON_ENERGY):                  self->nCurrentMoonEnergy += it.param1Int; break;
            case INT(Type::SUN_POWER_VALUE):                      self->nSunPowerValue = it.param1Int; break;
            case INT(Type::MOON_POWER_VALUE):                     self->nMoonPowerValue = it.param1Int; break;
            case INT(Type::CALL_BUFF):                            self->buffAdd(self->dwID, self->chAttr.atLevel, it.param1Int, it.param2); break;
            case INT(Type::DEL_SINGLE_BUFF_BY_ID_AND_LEVEL):      self->buffDel(it.param1Int, it.param2); break;
            case INT(Type::DEL_MULTI_GROUP_BUFF_BY_FUNCTIONTYPE): break; // 未做相关实现, 目前其仅在解控时使用
            case INT(Type::DEL_MULTI_GROUP_BUFF_BY_ID):           self->buffDelMultiGroupByID(it.param1Int); break;
            case INT(Type::STOP):                                 break; // 未做相关实现, 推测为停止, 用于解除击飞
            case INT(Type::DO_ACTION):                            break;                            // 未做相关实现, 推测为动作
            default:
                CONSTEXPR_LOG_ERROR("Undefined: {}, {}: {} {}, rollback={}", Ref<Mode>::names[it.mode], Ref<Type>::names[it.type], it.param1Int, it.param2, isRollback);
                break;
            }
        } break; // EFFECT_TO_SELF_NOT_ROLLBACK

        case INT(Mode::EFFECT_TO_DEST_NOT_ROLLBACK): {
            if (target == nullptr) // TO_DEST
                break;
            if (isRollback) // NOT_ROLLBACK
                break;
            switch (it.type) {
            case INT(Type::CALL_PHYSICS_DAMAGE):  proxyRecipe(callDamage, this, true, DamageType::Physics, atPhysicsDamage, atPhysicsDamageRand, false); break;
            case INT(Type::CALL_SOLAR_DAMAGE):    proxyRecipe(callDamage, this, true, DamageType::Solar, atSolarDamage, atSolarDamageRand, false); break;
            case INT(Type::CALL_NEUTRAL_DAMAGE):  proxyRecipe(callDamage, this, true, DamageType::Neutral, atNeutralDamage, atNeutralDamageRand, false); break;
            case INT(Type::CALL_LUNAR_DAMAGE):    proxyRecipe(callDamage, this, true, DamageType::Lunar, atLunarDamage, atLunarDamageRand, false); break;
            case INT(Type::CALL_POISON_DAMAGE):   proxyRecipe(callDamage, this, true, DamageType::Poison, atPoisonDamage, atPoisonDamageRand, false); break;
            case INT(Type::CALL_ADAPTIVE_DAMAGE): {
                switch (self->atAdaptiveSkillType) {
                case ref::lua::SKILL_KIND_TYPE::PHYSICS:       proxyRecipe(callDamage, this, true, DamageType::Physics, atPhysicsDamage, atPhysicsDamageRand, false); break;
                case ref::lua::SKILL_KIND_TYPE::SOLAR_MAGIC:   proxyRecipe(callDamage, this, true, DamageType::Solar, atSolarDamage, atSolarDamageRand, false); break;
                case ref::lua::SKILL_KIND_TYPE::NEUTRAL_MAGIC: proxyRecipe(callDamage, this, true, DamageType::Neutral, atNeutralDamage, atNeutralDamageRand, false); break;
                case ref::lua::SKILL_KIND_TYPE::LUNAR_MAGIC:   proxyRecipe(callDamage, this, true, DamageType::Lunar, atLunarDamage, atLunarDamageRand, false); break;
                case ref::lua::SKILL_KIND_TYPE::POISON_MAGIC:  proxyRecipe(callDamage, this, true, DamageType::Poison, atPoisonDamage, atPoisonDamageRand, false); break;
                default:                                       break;
                }
            } break;
            case INT(Type::CALL_SURPLUS_PHYSICS_DAMAGE): proxyRecipe(callDamage, this, true, DamageType::Physics, 0, 0, true); break;
            case INT(Type::CALL_SURPLUS_SOLAR_DAMAGE):   proxyRecipe(callDamage, this, true, DamageType::Solar, 0, 0, true); break;
            case INT(Type::CALL_SURPLUS_NEUTRAL_DAMAGE): proxyRecipe(callDamage, this, true, DamageType::Neutral, 0, 0, true); break;
            case INT(Type::CALL_SURPLUS_LUNAR_DAMAGE):   proxyRecipe(callDamage, this, true, DamageType::Lunar, 0, 0, true); break;
            case INT(Type::CALL_SURPLUS_POISON_DAMAGE):  proxyRecipe(callDamage, this, true, DamageType::Poison, 0, 0, true); break;
            case INT(Type::EXECUTE_SCRIPT):              executeScript(this, true, it.param1Str, std::nullopt, false); break;
            case INT(Type::EXECUTE_SCRIPT_WITH_PARAM):   executeScript(this, true, it.param1Str, it.param2, false); break;
            case INT(Type::CALL_BUFF):                   target->buffAdd(self->dwID, self->chAttr.atLevel, it.param1Int, it.param2); break;
            case INT(Type::DASH):                        break; // 未做相关实现, 推测为冲刺
            default:
                CONSTEXPR_LOG_ERROR("Undefined: {}, {}: {} {}, rollback={}", Ref<Mode>::names[it.mode], Ref<Type>::names[it.type], it.param1Int, it.param2, isRollback);
                break;
            }
        } break; // EFFECT_TO_DEST_NOT_ROLLBACK

        default:
            break;

        } // switch (it.mode)
    } // for (auto &it : skill.attrAttributes)
}
