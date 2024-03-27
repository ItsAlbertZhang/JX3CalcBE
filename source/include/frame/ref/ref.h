#pragma once

#include <string>
#include <unordered_map>

namespace jx3calc {
namespace frame {

enum class refEnum {
    Attrib,
    // Skill
    Skill_Kindtype, // 目前仅实现了此项
    // SkillEvent
    SkillEvent_EventType,
    SkillEvent_CasterTarget,
    // lua
    lua_ATTRIBUTE_TYPE,
    // 特殊情况, lua special
    lua_KUNGFU_ADAPTIVETYPE_LIST,
    lua_MOVE_STATE,
    // 一般情况, lua normal
    lua_ATTRIBUTE_EFFECT_MODE,
    lua_BUFF_COMPARE_FLAG,
    lua_SKILL_COMPARE_FLAG,
    lua_TARGET,
    lua_SKILL_KIND_TYPE,
    lua_ROLE_TYPE,
    lua_ABSORB_ATTRIBUTE_SHIELD_TYPE,
};

template <refEnum r>
class Ref {
public:
    enum class Type {};
    static const char                                 *list[];
    static const std::unordered_map<std::string, Type> map;
};

} // namespace frame
} // namespace jx3calc
