#include "frame/character/helper/auto_rollback_attrib.h"
#include "frame/character/character.h"
#include "frame/lua/interface.h"
#include "frame/ref/tab_attribute.h" // ref::Attrib
#include "plugin/log.h"
#include <random>

using namespace jx3calc;
using namespace frame;

AutoRollbackAttrib::AutoRollbackAttrib(Character *self, BuffItem *item, const Buff &buff) :
    self(self), item(item), buff(buff) {
    load();
}

AutoRollbackAttrib::~AutoRollbackAttrib() {
    self->buffFlushLeftFrame(item);
    for (const auto &it : buff.BeginAttrib) {
        handle(it, true);
    }
    for (const auto &it : buff.EndTimeAttrib) {
        handle(it, false);
    }
    if (!buff.ScriptFile.empty()) {
        std::string paramStr = "scripts/skill/" + buff.ScriptFile;
        if (!lua::interface::analysis(
                lua::interface::getOnRemove(paramStr)(
                    item->nCharacterID, item->nID, item->nLevel, item->nLeftFrame, item->nCustomValue, item->dwSkillSrcID, item->nStackNum, 0, 0, item->dwCasterSkillID
                ),
                paramStr,
                lua::interface::FuncType::OnRemove
            ))
            CONSTEXPR_LOG_ERROR("LuaFunc::getOnRemove(\"{}\") failed.", paramStr);
        // OnRemove(nCharacterID, BuffID, nBuffLevel, nLeftFrame, nCustomValue, dwSkillSrcID, nStackNum, nBuffIndex, dwCasterID, dwCasterSkillID)
    }
}
void AutoRollbackAttrib::active() {
    self->buffFlushLeftFrame(item);
    for (const auto &it : buff.ActiveAttrib) {
        handle(it, false);
    }
}
void AutoRollbackAttrib::load() {
    for (const auto &it : buff.BeginAttrib) {
        handle(it, false);
    }
}
void AutoRollbackAttrib::unload() {
    for (const auto &it : buff.BeginAttrib) {
        handle(it, true);
    }
}

void AutoRollbackAttrib::handle(const Buff::Attrib &attrib, bool isRollback) {
    int c     = isRollback ? -1 : 1;
    int stack = item->nStackNum;
    switch (attrib.type) {
    case ref::Attrib::atCallSolarDamage: {
        // 计算会心
        Character *src                                 = Character::characterGet(item->dwSkillSrcID);
        // 注意计算会心时使用的是 item->attr, 而不是 src->chAttr, 实现快照效果
        auto [atCriticalStrike, atCriticalDamagePower] = src->calcCritical(item->attr, item->dwCasterSkillID, item->dwCasterSkillLevel);
        std::random_device              rd;
        std::mt19937                    gen(rd());
        std::uniform_int_distribution<> dis(0, 9999);
        bool                            isCritical = dis(gen) < atCriticalStrike;
        // 注意计算伤害时使用的是 item->attr, 而不是 src->chAttr, 实现快照效果
        src->chDamage.emplace_back(src->calcDamage(
            item->nID,
            item->nLevel,
            item->attr,
            self,
            DamageType::Solar,
            atCriticalStrike,
            atCriticalDamagePower,
            attrib.valueAInt,
            0,
            0,
            item->nChannelInterval,
            0,
            isCritical,
            false,
            true,
            item->rawInterval,
            item->rawCount
        ));
        src->bFightState = true;
    } break;
    case ref::Attrib::atCallLunarDamage: {
        // 计算会心
        Character *src                                 = Character::characterGet(item->dwSkillSrcID);
        // 注意计算会心时使用的是 item->attr, 而不是 src->chAttr, 实现快照效果
        auto [atCriticalStrike, atCriticalDamagePower] = src->calcCritical(item->attr, item->dwCasterSkillID, item->dwCasterSkillLevel);
        std::random_device              rd;
        std::mt19937                    gen(rd());
        std::uniform_int_distribution<> dis(0, 9999);
        bool                            isCritical = dis(gen) < atCriticalStrike;
        // 注意计算伤害时使用的是 item->attr, 而不是 src->chAttr, 实现快照效果
        src->chDamage.emplace_back(src->calcDamage(
            item->nID,
            item->nLevel,
            item->attr,
            self,
            DamageType::Lunar,
            atCriticalStrike,
            atCriticalDamagePower,
            attrib.valueAInt,
            0,
            0,
            item->nChannelInterval,
            0,
            isCritical,
            false,
            true,
            item->rawInterval,
            item->rawCount
        ));
        src->bFightState = true;
    } break;
    case ref::Attrib::atExecuteScript: {
        std::string paramStr = "scripts/" + attrib.valueAStr;
        if (isRollback) {
            if (!lua::interface::analysis(lua::interface::getUnApply(paramStr)(item->nCharacterID, item->dwSkillSrcID), paramStr, lua::interface::FuncType::UnApply))
                CONSTEXPR_LOG_ERROR("LuaFunc::getUnApply(\"{}\") failed.", paramStr);
        } else {
            if (!lua::interface::analysis(lua::interface::getApply(paramStr)(item->nCharacterID, item->dwSkillSrcID), paramStr, lua::interface::FuncType::Apply))
                CONSTEXPR_LOG_ERROR("LuaFunc::getApply(\"{}\") failed.", paramStr);
        }
    } break;
    case ref::Attrib::atSetTalentRecipe:
        if (isRollback) {
            self->skillrecipeRemove(attrib.valueAInt, attrib.valueBInt);
        } else {
            self->skillrecipeAdd(attrib.valueAInt, attrib.valueBInt);
        }
        break;
    case ref::Attrib::atSkillEventHandler:
        if (isRollback) {
            self->skilleventRemove(attrib.valueAInt);
        } else {
            self->skilleventAdd(attrib.valueAInt);
        }
        break;
    case ref::Attrib::atCallBuff:                                  self->buffAdd(0, 99, attrib.valueAInt, attrib.valueBInt); break;
    case ref::Attrib::atCastSkillTargetDst:                        self->skillCast(attrib.valueAInt, attrib.valueBInt); break;
    // 并列属性
    case ref::Attrib::atBasePotentialAdd:                          self->chAttr.atBasePotentialAdd += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atSpunkBase:                                 self->chAttr.atSpunkBase += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atAgilityBasePercentAdd:                     self->chAttr.atAgilityBasePercentAdd += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atSpiritBasePercentAdd:                      self->chAttr.atSpiritBasePercentAdd += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atSpunkBasePercentAdd:                       self->chAttr.atSpunkBasePercentAdd += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atStrengthBasePercentAdd:                    self->chAttr.atStrengthBasePercentAdd += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atAllDamageAddPercent:                       self->chAttr.atAllDamageAddPercent += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atAllPhysicsDamageAddPercent:                self->chAttr.atAllPhysicsDamageAddPercent += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atAllMagicDamageAddPercent:                  self->chAttr.atAllMagicDamageAddPercent += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atAllTypeCriticalStrike:                     self->chAttr.atAllTypeCriticalStrike += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atPhysicsCriticalStrikeBaseRate:             self->chAttr.atPhysicsCriticalStrikeBaseRate += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atSolarCriticalStrikeBaseRate:               self->chAttr.atSolarCriticalStrikeBaseRate += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atNeutralCriticalStrikeBaseRate:             self->chAttr.atNeutralCriticalStrikeBaseRate += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atLunarCriticalStrikeBaseRate:               self->chAttr.atLunarCriticalStrikeBaseRate += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atPoisonCriticalStrikeBaseRate:              self->chAttr.atPoisonCriticalStrikeBaseRate += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atMagicAttackPowerBase:                      self->chAttr.atMagicAttackPowerBase += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atPhysicsAttackPowerPercent:                 self->chAttr.atPhysicsAttackPowerPercent += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atMagicAttackPowerPercent:                   self->chAttr.atMagicAttackPowerPercent += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atSolarAttackPowerPercent:                   self->chAttr.atSolarAttackPowerPercent += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atNeutralAttackPowerPercent:                 self->chAttr.atNeutralAttackPowerPercent += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atLunarAttackPowerPercent:                   self->chAttr.atLunarAttackPowerPercent += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atPoisonAttackPowerPercent:                  self->chAttr.atPoisonAttackPowerPercent += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atPhysicsCriticalDamagePowerBaseKiloNumRate: self->chAttr.atPhysicsCriticalDamagePowerBaseKiloNumRate += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atMagicCriticalDamagePowerBaseKiloNumRate:   self->chAttr.atMagicCriticalDamagePowerBaseKiloNumRate += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atSolarCriticalDamagePowerBaseKiloNumRate:   self->chAttr.atSolarCriticalDamagePowerBaseKiloNumRate += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atLunarCriticalDamagePowerBaseKiloNumRate:   self->chAttr.atLunarCriticalDamagePowerBaseKiloNumRate += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atPhysicsOvercomeBase:                       self->chAttr.atPhysicsOvercomeBase += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atMagicOvercome:                             self->chAttr.atMagicOvercome += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atPhysicsOvercomePercent:                    self->chAttr.atPhysicsOvercomePercent += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atSolarOvercomePercent:                      self->chAttr.atSolarOvercomePercent += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atNeutralOvercomePercent:                    self->chAttr.atNeutralOvercomePercent += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atLunarOvercomePercent:                      self->chAttr.atLunarOvercomePercent += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atPoisonOvercomePercent:                     self->chAttr.atPoisonOvercomePercent += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atMagicShield:                               self->chAttr.atMagicShield += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atSolarMagicShieldPercent:                   self->chAttr.atSolarMagicShieldPercent += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atNeutralMagicShieldPercent:                 self->chAttr.atNeutralMagicShieldPercent += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atLunarMagicShieldPercent:                   self->chAttr.atLunarMagicShieldPercent += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atPoisonMagicShieldPercent:                  self->chAttr.atPoisonMagicShieldPercent += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atPhysicsDamageCoefficient:                  self->chAttr.atPhysicsDamageCoefficient += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atSolarDamageCoefficient:                    self->chAttr.atSolarDamageCoefficient += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atNeutralDamageCoefficient:                  self->chAttr.atNeutralDamageCoefficient += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atLunarDamageCoefficient:                    self->chAttr.atLunarDamageCoefficient += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atPoisonDamageCoefficient:                   self->chAttr.atPoisonDamageCoefficient += attrib.valueAInt * c * stack; break;
    // 单列属性
    case ref::Attrib::atAllShieldIgnorePercent:                    self->chAttr.atAllShieldIgnorePercent += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atDstNpcDamageCoefficient:                   self->chAttr.atDstNpcDamageCoefficient += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atHasteBase:                                 self->chAttr.atHasteBase += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atSurplusValueBase:                          self->chAttr.atSurplusValueBase += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atStrainBase:                                self->chAttr.atStrainBase += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atStrainRate:                                self->chAttr.atStrainRate += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atToughnessBaseRate:                         self->chAttr.atToughnessBaseRate += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atTherapyPowerBase:                          self->chAttr.atTherapyPowerBase += attrib.valueAInt * c * stack; break;
    case ref::Attrib::atBeTherapyCoefficient:                      self->chAttr.atBeTherapyCoefficient += attrib.valueAInt * c * stack; break;
    // 无关紧要的属性
    case ref::Attrib::atTransferDamage:                            break;
    case ref::Attrib::atFormationEffect:                           break;
    case ref::Attrib::atAddExpPercent:                             break;
    case ref::Attrib::atAddReputationPercent:                      break;
    case ref::Attrib::atKnockedBackRate:                           break;
    case ref::Attrib::atRepulsedRate:                              break;
    case ref::Attrib::atDamageToLifeForSelf:                       break;
    case ref::Attrib::atKnockedOffRate:                            break;
    case ref::Attrib::atStealth:                                   break; // 隐身
    case ref::Attrib::atMoveSpeedPercent:                          break;
    case ref::Attrib::atKnockedDownRate:                           break;
    case ref::Attrib::atBeImmunisedStealthEnable:                  break;
    case ref::Attrib::atImmunity:                                  break;
    case ref::Attrib::atImmuneSkillMove:                           break;
    case ref::Attrib::atActiveThreatCoefficient:                   break;
    case ref::Attrib::atHalt:                                      break; // 禁止移动
    case ref::Attrib::atNoLimitChangeSkillIcon:                    break;
    case ref::Attrib::atAddTransparencyValue:                      break; // 透明度
    case ref::Attrib::atSetSelectableType:                         break;
    case ref::Attrib::atGlobalResistPercent:                       break; // 减伤
    default:
        CONSTEXPR_LOG_ERROR("Undefined: {} {} Unknown Attribute: {} {}", item->nID, item->nLevel, Ref<ref::Attrib>::names[static_cast<int>(attrib.type)], attrib.valueAInt);
        break;
    }
}
