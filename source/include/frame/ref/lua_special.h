#pragma once

#include "frame/ref/ref.h"

namespace jx3calc {
namespace frame {

template <>
class Ref<ref::lua::KUNGFU_ADAPTIVETYPE_LIST> {
public:
    enum class Type {
        ZERO,
        lunar,
        solar,
        neutral,
        poison,
        physics,
        COUNT,
    };

    enum class Enum {
        MjFysj = 10242,
        COUNT,
    };

    inline static const std::unordered_map<Enum, Type> EnumToType = {
        {Enum::MjFysj, Type::lunar},
    };
};

template <>
// 几乎确定不全, 出现于 明教/明教_暗尘弥散.lua
class Ref<ref::lua::MOVE_STATE> {
public:
    enum class Type {
        ON_ENTRAP,
        ON_FREEZE,
        ON_HALT,
        ON_KNOCKED_BACK,
        ON_KNOCKED_DOWN,
        ON_KNOCKED_OFF,
        ON_SKILL_MOVE_DST,
        ON_REPULSED,
        COUNT,
    };

    inline static const char *list[]{
        "ON_ENTRAP",
        "ON_FREEZE",
        "ON_HALT",
        "ON_KNOCKED_BACK",
        "ON_KNOCKED_DOWN",
        "ON_KNOCKED_OFF",
        "ON_SKILL_MOVE_DST",
        "ON_REPULSED",
    };

    inline static const std::unordered_map<std::string, Type> map{};
};

} // namespace frame
} // namespace jx3calc
