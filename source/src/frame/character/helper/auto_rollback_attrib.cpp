#include "frame/character/helper/auto_rollback_attrib.h"
#include "frame/event.h"
#include "frame/runtime_lua.h"
#include "frame/static_refmap.h" // enumTabAttribute
#include "program/log.h"
#include <random>

using namespace ns_frame;
using namespace ns_framestatic;

AutoRollbackAttrib::AutoRollbackAttrib(Character *self, CharacterBuff::Item *item, const Buff &buff)
    : self(self), item(item), buff(buff) {
    for (const auto &it : buff.BeginAttrib) {
        handle(it, false);
    }
}

AutoRollbackAttrib::~AutoRollbackAttrib() {
    item->flushLeftFrame();
    for (const auto &it : buff.BeginAttrib) {
        handle(it, true);
    }
    for (const auto &it : buff.EndTimeAttrib) {
        handle(it, false);
    }
    if (!buff.ScriptFile.empty()) {
        std::string paramStr = "scripts/skill/" + buff.ScriptFile;
        LuaFunc::analysis(LuaFunc::getOnRemove(paramStr)(item->nCharacterID, item->BuffID, item->nBuffLevel, item->nLeftFrame, item->nCustomValue, item->dwSkillSrcID, item->nStackNum, 0, 0, item->dwCasterSkillID), paramStr, LuaFunc::Enum::OnRemove);
        // OnRemove(nCharacterID, BuffID, nBuffLevel, nLeftFrame, nCustomValue, dwSkillSrcID, nStackNum, nBuffIndex, dwCasterID, dwCasterSkillID)
    }
}
void AutoRollbackAttrib::active() {
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
    case enumTabAttribute::atCallLunarDamage: {
        // 计算会心
        Character *src = Character::getCharacter(item->dwSkillSrcID);
        auto [atCriticalStrike, atCriticalDamagePower] =
            src->CalcCritical(item->attr, item->dwCasterSkillID, item->dwCasterSkillLevel); // 注意这里使用的是 item->attr, 而不是 src->chAttr, 实现快照效果
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 9999);
        bool isCritical = dis(gen) < atCriticalStrike;
        src->chDamage.damageList.emplace_back(
            Event::now(),
            item->dwCasterSkillID, item->dwCasterSkillLevel,
            isCritical,
            src->CalcDamage(
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
            self->chSkillEvent.remove(attrib.valueAInt);
        } else {
            self->chSkillEvent.add(attrib.valueAInt);
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
    default:
        LOG_ERROR("Undefined: Unknown Attribute: %s %d\n", refTabAttribute[static_cast<int>(attrib.type)].c_str(), attrib.valueAInt);
        break;
    }
}