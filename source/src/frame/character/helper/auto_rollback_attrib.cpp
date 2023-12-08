#include "frame/character/helper/auto_rollback_attrib.h"
#include "frame/event.h"
#include "frame/lua_runtime.h"
#include "frame/ref/tab_attribute.h" // enumTabAttribute
#include "program/log.h"
#include <random>

using namespace ns_frame;
using namespace ns_frame::ref;

AutoRollbackAttrib::AutoRollbackAttrib(Character *self, CharacterBuff::Item *item, const Buff &buff)
    : self(self), item(item), buff(buff) {
    // item->flushLeftFrame();
    for (const auto &it : buff.BeginAttrib) {
        handle(item, it, false);
    }
}

AutoRollbackAttrib::~AutoRollbackAttrib() {
    self->buffFlushLeftFrame(item);
    for (const auto &it : buff.BeginAttrib) {
        handle(item, it, true);
    }
    for (const auto &it : buff.EndTimeAttrib) {
        handle(item, it, false);
    }
    if (!buff.ScriptFile.empty()) {
        std::string paramStr = "scripts/skill/" + buff.ScriptFile;
        LuaFunc::analysis(LuaFunc::getOnRemove(paramStr)(item->nCharacterID, item->BuffID, item->nLevel, item->nLeftFrame, item->nCustomValue, item->dwSkillSrcID, item->nStackNum, 0, 0, item->dwCasterSkillID), paramStr, LuaFunc::Enum::OnRemove);
        // OnRemove(nCharacterID, BuffID, nBuffLevel, nLeftFrame, nCustomValue, dwSkillSrcID, nStackNum, nBuffIndex, dwCasterID, dwCasterSkillID)
    }
}
void AutoRollbackAttrib::active() {
    self->buffFlushLeftFrame(item);
    for (const auto &it : buff.ActiveAttrib) {
        handle(item, it, false);
    }
}

void AutoRollbackAttrib::handle(CharacterBuff::Item *item, const Buff::Attrib &attrib, bool isRollback) {
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
        Character *src = Character::characterGet(item->dwSkillSrcID);
        auto [atCriticalStrike, atCriticalDamagePower] =
            src->calcCritical(item->attr, item->dwCasterSkillID, item->dwCasterSkillLevel); // 注意这里使用的是 item->attr, 而不是 src->chAttr, 实现快照效果
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 9999);
        bool isCritical = dis(gen) < atCriticalStrike;
        src->chDamage.damageList.emplace_back(
            Event::now(), DamageSource::buff,
            item->BuffID, item->nLevel,
            isCritical,
            src->calcDamage(
                item->attr, self, DamageType::Solar, // 注意这里使用的是 item->attr, 而不是 src->chAttr, 实现快照效果
                isCritical, atCriticalDamagePower, 0,
                attrib.valueAInt, 0,
                item->nChannelInterval, 0,
                item->rawInterval, item->rawCount),
            DamageType::Solar);
        src->bFightState = true;
    } break;
    case enumTabAttribute::atCallLunarDamage: {
        // 计算会心
        Character *src = Character::characterGet(item->dwSkillSrcID);
        auto [atCriticalStrike, atCriticalDamagePower] =
            src->calcCritical(item->attr, item->dwCasterSkillID, item->dwCasterSkillLevel); // 注意这里使用的是 item->attr, 而不是 src->chAttr, 实现快照效果
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 9999);
        bool isCritical = dis(gen) < atCriticalStrike;
        src->chDamage.damageList.emplace_back(
            Event::now(), DamageSource::buff,
            item->BuffID, item->nLevel,
            isCritical,
            src->calcDamage(
                item->attr, self, DamageType::Lunar, // 注意这里使用的是 item->attr, 而不是 src->chAttr, 实现快照效果
                isCritical, atCriticalDamagePower, 0,
                attrib.valueAInt, 0,
                item->nChannelInterval, 0,
                item->rawInterval, item->rawCount),
            DamageType::Lunar);
        src->bFightState = true;
    } break;
    case enumTabAttribute::atExecuteScript: {
        std::string paramStr = "scripts/" + attrib.valueAStr;
        if (isRollback) {
            LuaFunc::analysis(LuaFunc::getUnApply(paramStr)(item->nCharacterID, item->dwSkillSrcID), paramStr, LuaFunc::Enum::UnApply);
        } else {
            LuaFunc::analysis(LuaFunc::getApply(paramStr)(item->nCharacterID, item->dwSkillSrcID), paramStr, LuaFunc::Enum::Apply);
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
    default:
        LOG_ERROR("Undefined: %d %d Unknown Attribute: %s %d\n", item->BuffID, item->nLevel, refTabAttribute[static_cast<int>(attrib.type)].c_str(), attrib.valueAInt);
        break;
    }
}