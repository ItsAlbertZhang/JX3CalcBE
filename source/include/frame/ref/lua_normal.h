#pragma once

#include "frame/ref/ref.h"

namespace jx3calc {
namespace frame {

enum class ref::lua::ABSORB_ATTRIBUTE_SHIELD_TYPE {
    GLOBAL,
    PHYSICS,
    SOLAR,
    NEUTRAL,
    LUNAR,
    POISON,
    COUNT,
};

enum class ref::lua::ATTRIBUTE_EFFECT_MODE {
    EFFECT_TO_SELF_NOT_ROLLBACK,
    EFFECT_TO_SELF_AND_ROLLBACK,
    EFFECT_TO_DEST_NOT_ROLLBACK,
    EFFECT_TO_DEST_AND_ROLLBACK,
    COUNT,
};

enum class ref::lua::BUFF_COMPARE_FLAG {
    EQUAL,
    GREATER,
    GREATER_EQUAL,
    COUNT,
};

enum class ref::lua::SKILL_COMPARE_FLAG {
    EQUAL,
    GREATER,
    GREATER_EQUAL,
    COUNT,
};

enum class ref::lua::TARGET {
    NPC,
    PLAYER,
    COUNT,
};

enum class ref::lua::SKILL_KIND_TYPE {
    PHYSICS,
    SOLAR_MAGIC,
    NEUTRAL_MAGIC,
    LUNAR_MAGIC,
    POISON_MAGIC,
    COUNT,
};

enum class ref::lua::ROLE_TYPE {
    LITTLE_BOY,
    STANDARD_MALE,
    LITTLE_GIRL,
    STANDARD_FEMALE,
    COUNT,
};

enum class ref::lua::PLAYER_ARENA_TYPE {
    DPS,
    THERAPY,
    T,
    COUNT,
};

} // namespace frame
} // namespace jx3calc
