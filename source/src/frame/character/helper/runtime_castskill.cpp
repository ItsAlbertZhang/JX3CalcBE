#include "frame/character/helper/runtime_castskill.h"
#include "frame/character/character.h"
#include "frame/lua/interface.h"
#include "frame/ref/lua_attribute_type.h"
#include "plugin/log.h"

using namespace jx3calc;
using namespace frame;

RuntimeCastSkill::~RuntimeCastSkill() {
    // 执行技能队列
    while (!skillQueue.empty()) {
        auto it = skillQueue.front();
        skillQueue.pop();
        switch (it.mode) {
        case static_cast<int>(ref::lua::ATTRIBUTE_EFFECT_MODE::EFFECT_TO_SELF_NOT_ROLLBACK): {
            switch (it.type) {
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CAST_SKILL_TARGET_DST):
                self->skillCast(self->targetCurr, it.param1Int, it.param2);
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::CAST_SKILL):
                self->skillCast(self->targetCurr, it.param1Int, it.param2);
                break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::EXECUTE_SCRIPT): {
                std::string paramStr      = "scripts/" + it.param1Str;
                int         dwCharacterID = Character::characterGetID(self);
                int         dwSkillSrcID  = Character::characterGetID(self);
                if (!lua::interface::analysis(lua::interface::getApply(paramStr)(dwCharacterID, dwSkillSrcID), paramStr, lua::interface::FuncType::Apply))
                    CONSTEXPR_LOG_ERROR("LuaFunc::getApply(\"{}\") failed.", paramStr);
            } break;
            case static_cast<int>(ref::lua::ATTRIBUTE_TYPE::EXECUTE_SCRIPT_WITH_PARAM): {
                std::string paramStr      = "scripts/" + it.param1Str;
                int         dwCharacterID = Character::characterGetID(self);
                int         dwSkillSrcID  = Character::characterGetID(self);
                if (!lua::interface::analysis(lua::interface::getApply(paramStr)(dwCharacterID, it.param2, dwSkillSrcID), paramStr, lua::interface::FuncType::Apply))
                    CONSTEXPR_LOG_ERROR("LuaFunc::getApply(\"{}\") failed.", paramStr);
            } break;
            default:
                break;
            }
        } break;
        default:
            break;
        }
    }
    self->chDamage.insert(self->chDamage.end(), damageList.begin(), damageList.end());
}
