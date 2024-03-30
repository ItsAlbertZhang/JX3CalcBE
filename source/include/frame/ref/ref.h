#pragma once

#include <string>
#include <unordered_map>

namespace jx3calc {
namespace frame {

enum class refEnum {
    Attrib,
    // Skill
    Skill_KindType, // 目前仅实现了此项
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

struct ref {
    static const refEnum Attrib = refEnum::Attrib;
    struct Skill {
        static const refEnum KindType = refEnum::Skill_KindType;
    };
    struct SkillEvent {
        static const refEnum EventType    = refEnum::SkillEvent_EventType;
        static const refEnum CasterTarget = refEnum::SkillEvent_CasterTarget;
    };
    struct lua {
        static const refEnum ATTRIBUTE_TYPE               = refEnum::lua_ATTRIBUTE_TYPE;
        static const refEnum KUNGFU_ADAPTIVETYPE_LIST     = refEnum::lua_KUNGFU_ADAPTIVETYPE_LIST;
        static const refEnum MOVE_STATE                   = refEnum::lua_MOVE_STATE;
        static const refEnum ATTRIBUTE_EFFECT_MODE        = refEnum::lua_ATTRIBUTE_EFFECT_MODE;
        static const refEnum BUFF_COMPARE_FLAG            = refEnum::lua_BUFF_COMPARE_FLAG;
        static const refEnum SKILL_COMPARE_FLAG           = refEnum::lua_SKILL_COMPARE_FLAG;
        static const refEnum TARGET                       = refEnum::lua_TARGET;
        static const refEnum SKILL_KIND_TYPE              = refEnum::lua_SKILL_KIND_TYPE;
        static const refEnum ROLE_TYPE                    = refEnum::lua_ROLE_TYPE;
        static const refEnum ABSORB_ATTRIBUTE_SHIELD_TYPE = refEnum::lua_ABSORB_ATTRIBUTE_SHIELD_TYPE;
    };
};

template <refEnum r>
class Ref {
public:
    enum class Type;
    static const char                                 *list[];
    static const std::unordered_map<std::string, Type> map;
};

} // namespace frame
} // namespace jx3calc
