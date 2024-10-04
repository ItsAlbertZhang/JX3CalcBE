#include "frame/character/helper/auto_rollback_attribute.h"
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

AutoRollbackAttribute::AutoRollbackAttribute(
    Character *self, Character *target, AutoRollbackAttribute *ancestor, const Skill &skill, const std::vector<const Skill *> *skillrecipeList,
    int skillID, int skillLevel, int damageAddPercent
) :
    self(self), target(target), ancestor(ancestor ? ancestor : this), skill(skill), skillrecipeList(skillrecipeList),
    skillID(skillID), skillLevel(skillLevel), damageAddPercent(damageAddPercent) // constructor
{

    loadRecipe();
    {
        std::tuple<int, int> res  = this->self->calcCritical(this->self->chAttr, this->skillID, this->skillLevel);
        this->criticalStrike      = std::get<0>(res);
        this->criticalDamagePower = std::get<1>(res);
        std::random_device              rd;
        std::mt19937                    gen(rd());
        std::uniform_int_distribution<> dis(0, 9999);
        this->isCritical = dis(gen) < this->criticalStrike;
    }
    unloadRecipe();
    handle(false);
}

AutoRollbackAttribute::~AutoRollbackAttribute() {
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

bool AutoRollbackAttribute::getCritical() const {
    return this->isCritical;
}

std::tuple<int, int> &AutoRollbackAttribute::emplace(int skillID, int skillLevel) {
    return skillQueue.emplace(std::make_tuple(skillID, skillLevel));
}

void AutoRollbackAttribute::loadRecipe() {
    if (skillrecipeList == nullptr)
        return;
    this->recipeSkills.clear();
    this->recipeSkills.reserve(skillrecipeList->size());
    for (const auto &it : *skillrecipeList) {
        this->recipeSkills.emplace_back(std::make_unique<AutoRollbackAttribute>(self, target, ancestor, *it, nullptr, skillID, skillLevel, 0));
    }
}

void AutoRollbackAttribute::unloadRecipe() {
    this->recipeSkills.clear();
}

void AutoRollbackAttribute::handle(bool isRollback) {
    const auto callDamage = [this](bool isDest, DamageType type, const int *ptrDamageBase, const int *ptrDamageRand, bool isSurplus) -> void {
        if (isDest && this->target == nullptr) [[unlikely]]
            return;
        this->loadRecipe();
        Character *target     = isDest ? this->target : this->self;
        const int  damageBase = ptrDamageBase ? *ptrDamageBase : 0;
        const int  damageRand = ptrDamageRand ? *ptrDamageRand : 0;

        this->self->bFightState = true;
        this->ancestor->damage += this->self->calcDamage(
            this->skill.dwSkillID,
            this->skill.dwLevel,
            this->self->chAttr,
            target,
            type,
            this->criticalStrike,
            this->criticalDamagePower,
            damageBase,
            damageRand,
            this->damageAddPercent,
            isSurplus ? 1 : static_cast<int>(this->skill.nChannelInterval),
            isSurplus ? 0 : this->skill.nWeaponDamagePercent,
            this->isCritical,
            isSurplus,
            false,
            1,
            1,
            this->skill.IsFrost
        );
        this->unloadRecipe();
    };
    const auto executeScript = [this](bool isDest, const std::string &param1Str, std::optional<int> param2, bool isRollback) -> void {
        if (isDest && this->target == nullptr) [[unlikely]]
            return;
        auto target        = isDest ? this->target : this->self;
        auto filename      = "scripts/" + param1Str;
        auto luaFunc       = isRollback ? lua::interface::getUnApply : lua::interface::getApply;
        auto dwCharacterID = Character::characterGetID(target);
        auto dwSkillSrcID  = Character::characterGetID(this->self);
        auto res =
            param2.has_value()
                ? luaFunc(filename)(dwCharacterID, param2.value(), dwSkillSrcID)
                : luaFunc(filename)(dwCharacterID, dwSkillSrcID);
        if (!lua::interface::analysis(std::move(res), filename, lua::interface::FuncType::Apply))
            CONSTEXPR_LOG_ERROR("LuaFunc::getApply(\"{}\") failed.", filename);
    };
    // const auto visitor = [&callDamage, &executeScript](auto &&arg) {
    //     using T = std::decay_t<decltype(arg)>;
    //     if constexpr (std::is_same_v<T, ItemCallDamage>) {
    //         callDamage(arg.isDest, arg.type, arg.ptrDamageBase, arg.ptrDamageRand, arg.isSurplus);
    //     } else if constexpr (std::is_same_v<T, ItemExecuteScript>) {
    //         executeScript(arg.isDest, arg.param1Str, arg.param2, arg.isRollback);
    //     }
    // };
    // std::queue<std::variant<ItemCallDamage, ItemExecuteScript>> queue;

    int c = isRollback ? -1 : 1;
    for (auto &it : skill.attrAttributes) {
        switch (it.mode) {

        case static_cast<int>(ref::lua::ATTRIBUTE_EFFECT_MODE::EFFECT_TO_SELF_AND_ROLLBACK): {
            switch (it.type) {
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::DROP_DEFENCE):                                  break; // 未做相关实现, 推测为摔落保护
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::ACTIVE_THREAT_COEFFICIENT):                     break; // 未做相关实现, 推测为威胁值
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::BEAT_BACK_RATE):                                break; // 未做相关实现, 推测为运功被打退概率
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::MAX_LIFE_PERCENT_ADD):                          break; // 未做相关实现, 额外最大生命值
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::MANA_REPLENISH_PERCENT):                        break; // 未做相关实现, 推测为内力回复
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::KUNGFU_TYPE):                                   break; // 未做相关实现, 推测为武学类型, 用于心法 lua 中转换全能属性
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_PHYSICS_DAMAGE):                          this->atPhysicsDamage += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_PHYSICS_DAMAGE_RAND):                     this->atPhysicsDamageRand += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_SOLAR_DAMAGE):                            this->atSolarDamage += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_SOLAR_DAMAGE_RAND):                       this->atSolarDamageRand += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_NEUTRAL_DAMAGE):                          this->atNeutralDamage += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_NEUTRAL_DAMAGE_RAND):                     this->atNeutralDamageRand += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_LUNAR_DAMAGE):                            this->atLunarDamage += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_LUNAR_DAMAGE_RAND):                       this->atLunarDamageRand += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_POISON_DAMAGE):                           this->atPoisonDamage += it.param1Int * c; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SKILL_POISON_DAMAGE_RAND):                      this->atPoisonDamageRand += it.param1Int * c; break;
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
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::EXECUTE_SCRIPT):                                executeScript(false, it.param1Str, std::nullopt, isRollback); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::EXECUTE_SCRIPT_WITH_PARAM):                     executeScript(false, it.param1Str, it.param2, isRollback); break;
            // case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::EXECUTE_SCRIPT):                                queue.emplace(ItemExecuteScript {false, it.param1Str, std::nullopt, isRollback}); break;
            // case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::EXECUTE_SCRIPT_WITH_PARAM):                     queue.emplace(ItemExecuteScript {false, it.param1Str, it.param2, isRollback}); break;
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
    } // for (auto &it : skill.attrAttributes)

    for (auto &it : skill.attrAttributes) {
        switch (it.mode) {

        case static_cast<int>(ref::lua::ATTRIBUTE_EFFECT_MODE::EFFECT_TO_SELF_NOT_ROLLBACK): {
            if (isRollback) // NOT_ROLLBACK
                break;
            switch (it.type) {
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CAST_SKILL):            this->ancestor->skillQueue.emplace(std::make_tuple(it.param1Int, it.param2)); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CAST_SKILL_TARGET_DST): this->ancestor->skillQueue.emplace(std::make_tuple(it.param1Int, it.param2)); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CURRENT_SUN_ENERGY):    self->nCurrentSunEnergy += it.param1Int; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CURRENT_MOON_ENERGY):   self->nCurrentMoonEnergy += it.param1Int; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::SUN_POWER_VALUE):       self->nSunPowerValue = it.param1Int; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::MOON_POWER_VALUE):      self->nMoonPowerValue = it.param1Int; break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_BUFF):
                self->buffAdd(self->dwID, self->chAttr.atLevel, it.param1Int, it.param2);
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::DEL_SINGLE_BUFF_BY_ID_AND_LEVEL):
                self->buffDel(it.param1Int, it.param2);
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::DEL_MULTI_GROUP_BUFF_BY_FUNCTIONTYPE):
                break; // 未做相关实现, 目前其仅在解控时使用
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::DEL_MULTI_GROUP_BUFF_BY_ID):
                self->buffDelMultiGroupByID(it.param1Int);
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::STOP):                      break; // 未做相关实现, 推测为停止, 用于解除击飞
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::DO_ACTION):                 break; // 未做相关实现, 推测为动作
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::EXECUTE_SCRIPT):            executeScript(false, it.param1Str, std::nullopt, false); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::EXECUTE_SCRIPT_WITH_PARAM): executeScript(false, it.param1Str, it.param2, false); break;
            // case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::EXECUTE_SCRIPT):            queue.emplace(ItemExecuteScript {false, it.param1Str, std::nullopt, false}); break;
            // case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::EXECUTE_SCRIPT_WITH_PARAM): queue.emplace(ItemExecuteScript {false, it.param1Str, it.param2, false}); break;
            default:
                CONSTEXPR_LOG_ERROR("Undefined: {}, {}: {} {}, rollback={}", Ref<ref::lua::ATTRIBUTE_EFFECT_MODE>::names[it.mode], Ref<ref::lua::ATTRIBUTE_TYPE>::names[it.type], it.param1Int, it.param2, isRollback);
                break;
            }
        } break; // EFFECT_TO_SELF_NOT_ROLLBACK

        case static_cast<int>(ref::lua::ATTRIBUTE_EFFECT_MODE::EFFECT_TO_DEST_NOT_ROLLBACK): {
            if (target == nullptr) // TO_DEST
                break;
            if (isRollback) // NOT_ROLLBACK
                break;
            switch (it.type) {
            // case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_PHYSICS_DAMAGE):  queue.emplace(ItemCallDamage {true, DamageType::Physics, &atPhysicsDamage, &atPhysicsDamageRand, false}); break;
            // case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_SOLAR_DAMAGE):    queue.emplace(ItemCallDamage {true, DamageType::Solar, &atSolarDamage, &atSolarDamageRand, false}); break;
            // case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_NEUTRAL_DAMAGE):  queue.emplace(ItemCallDamage {true, DamageType::Neutral, &atNeutralDamage, &atNeutralDamageRand, false}); break;
            // case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_LUNAR_DAMAGE):    queue.emplace(ItemCallDamage {true, DamageType::Lunar, &atLunarDamage, &atLunarDamageRand, false}); break;
            // case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_POISON_DAMAGE):   queue.emplace(ItemCallDamage {true, DamageType::Poison, &atPoisonDamage, &atPoisonDamageRand, false}); break;
            // case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_ADAPTIVE_DAMAGE): {
            //     switch (self->atAdaptiveSkillType) {
            //     case ref::lua::SKILL_KIND_TYPE::PHYSICS:       queue.emplace(ItemCallDamage {true, DamageType::Physics, &atPhysicsDamage, &atPhysicsDamageRand, false}); break;
            //     case ref::lua::SKILL_KIND_TYPE::SOLAR_MAGIC:   queue.emplace(ItemCallDamage {true, DamageType::Solar, &atSolarDamage, &atSolarDamageRand, false}); break;
            //     case ref::lua::SKILL_KIND_TYPE::NEUTRAL_MAGIC: queue.emplace(ItemCallDamage {true, DamageType::Neutral, &atNeutralDamage, &atNeutralDamageRand, false}); break;
            //     case ref::lua::SKILL_KIND_TYPE::LUNAR_MAGIC:   queue.emplace(ItemCallDamage {true, DamageType::Lunar, &atLunarDamage, &atLunarDamageRand, false}); break;
            //     case ref::lua::SKILL_KIND_TYPE::POISON_MAGIC:  queue.emplace(ItemCallDamage {true, DamageType::Poison, &atPoisonDamage, &atPoisonDamageRand, false}); break;
            //     default:                                       break;
            //     }
            // } break;
            // case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_SURPLUS_PHYSICS_DAMAGE): queue.emplace(ItemCallDamage {true, DamageType::Physics, nullptr, nullptr, true}); break;
            // case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_SURPLUS_SOLAR_DAMAGE):   queue.emplace(ItemCallDamage {true, DamageType::Solar, nullptr, nullptr, true}); break;
            // case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_SURPLUS_NEUTRAL_DAMAGE): queue.emplace(ItemCallDamage {true, DamageType::Neutral, nullptr, nullptr, true}); break;
            // case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_SURPLUS_LUNAR_DAMAGE):   queue.emplace(ItemCallDamage {true, DamageType::Lunar, nullptr, nullptr, true}); break;
            // case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_SURPLUS_POISON_DAMAGE):  queue.emplace(ItemCallDamage {true, DamageType::Poison, nullptr, nullptr, true}); break;
            // case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::EXECUTE_SCRIPT):              queue.emplace(ItemExecuteScript {true, it.param1Str, std::nullopt, false}); break;
            // case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::EXECUTE_SCRIPT_WITH_PARAM):   queue.emplace(ItemExecuteScript {true, it.param1Str, it.param2, false}); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_PHYSICS_DAMAGE):  callDamage(true, DamageType::Physics, &atPhysicsDamage, &atPhysicsDamageRand, false); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_SOLAR_DAMAGE):    callDamage(true, DamageType::Solar, &atSolarDamage, &atSolarDamageRand, false); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_NEUTRAL_DAMAGE):  callDamage(true, DamageType::Neutral, &atNeutralDamage, &atNeutralDamageRand, false); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_LUNAR_DAMAGE):    callDamage(true, DamageType::Lunar, &atLunarDamage, &atLunarDamageRand, false); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_POISON_DAMAGE):   callDamage(true, DamageType::Poison, &atPoisonDamage, &atPoisonDamageRand, false); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_ADAPTIVE_DAMAGE): {
                switch (self->atAdaptiveSkillType) {
                case ref::lua::SKILL_KIND_TYPE::PHYSICS:       callDamage(true, DamageType::Physics, &atPhysicsDamage, &atPhysicsDamageRand, false); break;
                case ref::lua::SKILL_KIND_TYPE::SOLAR_MAGIC:   callDamage(true, DamageType::Solar, &atSolarDamage, &atSolarDamageRand, false); break;
                case ref::lua::SKILL_KIND_TYPE::NEUTRAL_MAGIC: callDamage(true, DamageType::Neutral, &atNeutralDamage, &atNeutralDamageRand, false); break;
                case ref::lua::SKILL_KIND_TYPE::LUNAR_MAGIC:   callDamage(true, DamageType::Lunar, &atLunarDamage, &atLunarDamageRand, false); break;
                case ref::lua::SKILL_KIND_TYPE::POISON_MAGIC:  callDamage(true, DamageType::Poison, &atPoisonDamage, &atPoisonDamageRand, false); break;
                default:                                       break;
                }
            } break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_SURPLUS_PHYSICS_DAMAGE): callDamage(true, DamageType::Physics, nullptr, nullptr, true); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_SURPLUS_SOLAR_DAMAGE):   callDamage(true, DamageType::Solar, nullptr, nullptr, true); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_SURPLUS_NEUTRAL_DAMAGE): callDamage(true, DamageType::Neutral, nullptr, nullptr, true); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_SURPLUS_LUNAR_DAMAGE):   callDamage(true, DamageType::Lunar, nullptr, nullptr, true); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_SURPLUS_POISON_DAMAGE):  callDamage(true, DamageType::Poison, nullptr, nullptr, true); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::EXECUTE_SCRIPT):              executeScript(true, it.param1Str, std::nullopt, false); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::EXECUTE_SCRIPT_WITH_PARAM):   executeScript(true, it.param1Str, it.param2, false); break;

            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CALL_BUFF):                   target->buffAdd(self->dwID, self->chAttr.atLevel, it.param1Int, it.param2); break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::DASH):                        break; // 未做相关实现, 推测为冲刺
            default:
                CONSTEXPR_LOG_ERROR("Undefined: {}, {}: {} {}, rollback={}", Ref<ref::lua::ATTRIBUTE_EFFECT_MODE>::names[it.mode], Ref<ref::lua::ATTRIBUTE_TYPE>::names[it.type], it.param1Int, it.param2, isRollback);
                break;
            }
        } break; // EFFECT_TO_DEST_NOT_ROLLBACK

        default:
            break;

        } // switch (it.mode)
    } // for (auto &it : skill.attrAttributes)

    // while (!queue.empty()) {
    //     auto item = std::move(queue.front());
    //     queue.pop();
    //     std::visit(visitor, item);
    // }
}
