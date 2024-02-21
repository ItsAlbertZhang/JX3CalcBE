#include "frame/character/helper/auto_rollback_attrib.h"
#include "frame/character/character.h"
#include "frame/lua_runtime.h"
#include "frame/ref/tab_attribute.h" // enumTabAttribute
#include "plugin/log.h"

using namespace ns_frame;
using namespace ns_frame::ref;

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
        if (!LuaFunc::analysis(
                LuaFunc::getOnRemove(paramStr)(
                    item->nCharacterID, item->nID, item->nLevel, item->nLeftFrame, item->nCustomValue, item->dwSkillSrcID, item->nStackNum, 0, 0, item->dwCasterSkillID
                ),
                paramStr,
                LuaFunc::Enum::OnRemove
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
    case enumTabAttribute::atLunarDamageCoefficient:
        self->chAttr.atLunarDamageCoefficient += attrib.valueAInt * c;
        break;
    case enumTabAttribute::atSolarDamageCoefficient:
        self->chAttr.atSolarDamageCoefficient += attrib.valueAInt * c;
        break;
    case enumTabAttribute::atCallSolarDamage: {
        // 计算会心
        Character *src                                 = Character::characterGet(item->dwSkillSrcID);
        // 注意计算会心时使用的是 item->attr, 而不是 src->chAttr, 实现快照效果
        auto [atCriticalStrike, atCriticalDamagePower] = src->calcCritical(item->attr, item->dwCasterSkillID, item->dwCasterSkillLevel);
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
            false,
            true,
            item->rawInterval,
            item->rawCount
        ));
        src->bFightState = true;
    } break;
    case enumTabAttribute::atCallLunarDamage: {
        // 计算会心
        Character *src                                 = Character::characterGet(item->dwSkillSrcID);
        // 注意计算会心时使用的是 item->attr, 而不是 src->chAttr, 实现快照效果
        auto [atCriticalStrike, atCriticalDamagePower] = src->calcCritical(item->attr, item->dwCasterSkillID, item->dwCasterSkillLevel);
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
            false,
            true,
            item->rawInterval,
            item->rawCount
        ));
        src->bFightState = true;
    } break;
    case enumTabAttribute::atExecuteScript: {
        std::string paramStr = "scripts/" + attrib.valueAStr;
        if (isRollback) {
            if (!LuaFunc::analysis(LuaFunc::getUnApply(paramStr)(item->nCharacterID, item->dwSkillSrcID), paramStr, LuaFunc::Enum::UnApply))
                CONSTEXPR_LOG_ERROR("LuaFunc::getUnApply(\"{}\") failed.", paramStr);
        } else {
            if (!LuaFunc::analysis(LuaFunc::getApply(paramStr)(item->nCharacterID, item->dwSkillSrcID), paramStr, LuaFunc::Enum::Apply))
                CONSTEXPR_LOG_ERROR("LuaFunc::getApply(\"{}\") failed.", paramStr);
        }
    } break;
    case enumTabAttribute::atLunarCriticalStrikeBaseRate:
        self->chAttr.atLunarCriticalStrikeBaseRate += attrib.valueAInt * c;
        break;
    case enumTabAttribute::atSolarCriticalStrikeBaseRate:
        self->chAttr.atSolarCriticalStrikeBaseRate += attrib.valueAInt * c;
        break;
    case enumTabAttribute::atMagicCriticalDamagePowerBaseKiloNumRate:
        self->chAttr.atMagicCriticalDamagePowerBaseKiloNumRate += attrib.valueAInt * c;
        break;
    case enumTabAttribute::atAllShieldIgnorePercent:
        self->chAttr.atAllShieldIgnorePercent += attrib.valueAInt * c;
        break;
    case enumTabAttribute::atAddTransparencyValue:
        // 未做相关实现, 推测为透明度
        break;
    case enumTabAttribute::atSetSelectableType:
        // 未做相关实现, 推测为是否可以选中
        break;
    case enumTabAttribute::atSkillEventHandler:
        if (isRollback) {
            self->skilleventRemove(attrib.valueAInt);
        } else {
            self->skilleventAdd(attrib.valueAInt);
        }
        break;
    case enumTabAttribute::atStealth:
        // 未做相关实现, 推测为隐身
        break;
    case enumTabAttribute::atMoveSpeedPercent:
        // 未做相关实现, 推测为移动速度
        break;
    case enumTabAttribute::atKnockedDownRate:
        // 未做相关实现, 推测为免疫击倒
        break;
    case enumTabAttribute::atBeImmunisedStealthEnable:
        // 未做相关实现
        break;
    case enumTabAttribute::atImmunity:
        // 未做相关实现
        break;
    case enumTabAttribute::atImmuneSkillMove:
        // 未做相关实现
        break;
    case enumTabAttribute::atActiveThreatCoefficient:
        // 未做相关实现, 推测为威胁值
        break;
    case enumTabAttribute::atHalt:
        // 未做相关实现, 推测为禁止移动
        break;
    case enumTabAttribute::atNoLimitChangeSkillIcon:
        // 未做相关实现, 推测为技能图标替换
        break;
    case enumTabAttribute::atSetTalentRecipe:
        if (isRollback) {
            self->skillrecipeRemove(attrib.valueAInt, attrib.valueBInt);
        } else {
            self->skillrecipeAdd(attrib.valueAInt, attrib.valueBInt);
        }
        break;
    case enumTabAttribute::atAllMagicDamageAddPercent:
        self->chAttr.atAllMagicDamageAddPercent += attrib.valueAInt * c;
        break;
    case enumTabAttribute::atBeTherapyCoefficient:
        self->chAttr.atBeTherapyCoefficient += attrib.valueAInt * c;
        break;
    case enumTabAttribute::atCallBuff:
        self->buffAdd4(0, 99, attrib.valueAInt, attrib.valueBInt);
        break;
    case enumTabAttribute::atKnockedOffRate:
        // 未做相关实现, 推测为免疫击退
        break;
    case enumTabAttribute::atSolarCriticalDamagePowerBaseKiloNumRate:
        self->chAttr.atSolarCriticalDamagePowerBaseKiloNumRate += attrib.valueAInt * c;
        break;
    case enumTabAttribute::atLunarCriticalDamagePowerBaseKiloNumRate:
        self->chAttr.atLunarCriticalDamagePowerBaseKiloNumRate += attrib.valueAInt * c;
        break;
    case enumTabAttribute::atAllDamageAddPercent:
        self->chAttr.atAllDamageAddPercent += attrib.valueAInt * c;
        break;
    default:
        CONSTEXPR_LOG_ERROR("Undefined: {} {} Unknown Attribute: {} {}", item->nID, item->nLevel, refTabAttribute[static_cast<int>(attrib.type)], attrib.valueAInt);
        break;
    }
}
