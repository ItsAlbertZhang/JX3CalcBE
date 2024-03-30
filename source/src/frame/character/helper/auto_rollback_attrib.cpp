#include "frame/character/helper/auto_rollback_attrib.h"
#include "frame/character/character.h"
#include "frame/lua/interface.h"
#include "frame/ref/tab_attribute.h" // ref::Attrib
#include "plugin/log.h"
#include <random>

using namespace jx3calc;
using namespace frame;

AutoRollbackAttrib::AutoRollbackAttrib(Character *self, BuffItem *item, const Buff &buff)
    : self(self), item(item), buff(buff) {
    // item->flushLeftFrame();
    for (const auto &it : buff.BeginAttrib) {
        handle(it, false);
    }
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

void AutoRollbackAttrib::handle(const Buff::Attrib &attrib, bool isRollback) {
    int c = isRollback ? -1 : 1;
    switch (attrib.type) {
    case ref::Attrib::atLunarDamageCoefficient:
        self->chAttr.atLunarDamageCoefficient += attrib.valueAInt * c;
        break;
    case ref::Attrib::atSolarDamageCoefficient:
        self->chAttr.atSolarDamageCoefficient += attrib.valueAInt * c;
        break;
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
    case ref::Attrib::atLunarCriticalStrikeBaseRate:
        self->chAttr.atLunarCriticalStrikeBaseRate += attrib.valueAInt * c;
        break;
    case ref::Attrib::atSolarCriticalStrikeBaseRate:
        self->chAttr.atSolarCriticalStrikeBaseRate += attrib.valueAInt * c;
        break;
    case ref::Attrib::atMagicCriticalDamagePowerBaseKiloNumRate:
        self->chAttr.atMagicCriticalDamagePowerBaseKiloNumRate += attrib.valueAInt * c;
        break;
    case ref::Attrib::atAllShieldIgnorePercent:
        self->chAttr.atAllShieldIgnorePercent += attrib.valueAInt * c;
        break;
    case ref::Attrib::atAddTransparencyValue:
        // 未做相关实现, 推测为透明度
        break;
    case ref::Attrib::atSetSelectableType:
        // 未做相关实现, 推测为是否可以选中
        break;
    case ref::Attrib::atSkillEventHandler:
        if (isRollback) {
            self->skilleventRemove(attrib.valueAInt);
        } else {
            self->skilleventAdd(attrib.valueAInt);
        }
        break;
    case ref::Attrib::atStealth:
        // 未做相关实现, 推测为隐身
        break;
    case ref::Attrib::atMoveSpeedPercent:
        // 未做相关实现, 推测为移动速度
        break;
    case ref::Attrib::atKnockedDownRate:
        // 未做相关实现, 推测为免疫击倒
        break;
    case ref::Attrib::atBeImmunisedStealthEnable:
        // 未做相关实现
        break;
    case ref::Attrib::atImmunity:
        // 未做相关实现
        break;
    case ref::Attrib::atImmuneSkillMove:
        // 未做相关实现
        break;
    case ref::Attrib::atActiveThreatCoefficient:
        // 未做相关实现, 推测为威胁值
        break;
    case ref::Attrib::atHalt:
        // 未做相关实现, 推测为禁止移动
        break;
    case ref::Attrib::atNoLimitChangeSkillIcon:
        // 未做相关实现, 推测为技能图标替换
        break;
    case ref::Attrib::atSetTalentRecipe:
        if (isRollback) {
            self->skillrecipeRemove(attrib.valueAInt, attrib.valueBInt);
        } else {
            self->skillrecipeAdd(attrib.valueAInt, attrib.valueBInt);
        }
        break;
    case ref::Attrib::atAllMagicDamageAddPercent:
        self->chAttr.atAllMagicDamageAddPercent += attrib.valueAInt * c;
        break;
    case ref::Attrib::atBeTherapyCoefficient:
        self->chAttr.atBeTherapyCoefficient += attrib.valueAInt * c;
        break;
    case ref::Attrib::atCallBuff:
        self->buffAdd(0, 99, attrib.valueAInt, attrib.valueBInt);
        break;
    case ref::Attrib::atKnockedOffRate:
        // 未做相关实现, 推测为免疫击退
        break;
    case ref::Attrib::atSolarCriticalDamagePowerBaseKiloNumRate:
        self->chAttr.atSolarCriticalDamagePowerBaseKiloNumRate += attrib.valueAInt * c;
        break;
    case ref::Attrib::atLunarCriticalDamagePowerBaseKiloNumRate:
        self->chAttr.atLunarCriticalDamagePowerBaseKiloNumRate += attrib.valueAInt * c;
        break;
    case ref::Attrib::atAllDamageAddPercent:
        self->chAttr.atAllDamageAddPercent += attrib.valueAInt * c;
        break;
    case ref::Attrib::atMagicOvercome:
        self->chAttr.atMagicOvercome += attrib.valueAInt * c;
        break;
    default:
        CONSTEXPR_LOG_ERROR("Undefined: {} {} Unknown Attribute: {} {}", item->nID, item->nLevel, Ref<ref::Attrib>::names[static_cast<int>(attrib.type)], attrib.valueAInt);
        break;
    }
}
