#pragma once

#include "frame/ref/ref.h"

namespace jx3calc {
namespace frame {

template <>
class Ref<ref::lua::ABSORB_ATTRIBUTE_SHIELD_TYPE> {
public:
    enum class Type {
        GLOBAL,
        PHYSICS,
        SOLAR,
        NEUTRAL,
        LUNAR,
        POISON,
        COUNT,
    };

    inline static const char *list[]{
        "GLOBAL",
        "PHYSICS",
        "SOLAR",
        "NEUTRAL",
        "LUNAR",
        "POISON",
    };

    inline static const std::unordered_map<std::string, Type> map{};
};

template <>
class Ref<ref::lua::ATTRIBUTE_EFFECT_MODE> {
public:
    enum class Type {
        EFFECT_TO_SELF_NOT_ROLLBACK,
        EFFECT_TO_SELF_AND_ROLLBACK,
        EFFECT_TO_DEST_NOT_ROLLBACK,
        EFFECT_TO_DEST_AND_ROLLBACK,
        COUNT,
    };

    inline static const char *list[]{
        "EFFECT_TO_SELF_NOT_ROLLBACK",
        "EFFECT_TO_SELF_AND_ROLLBACK",
        "EFFECT_TO_DEST_NOT_ROLLBACK",
        "EFFECT_TO_DEST_AND_ROLLBACK",
    };

    inline static const std::unordered_map<std::string, Type> map{};
};

template <>
class Ref<ref::lua::BUFF_COMPARE_FLAG> {
public:
    enum class Type {
        EQUAL,
        GREATER,
        GREATER_EQUAL,
        COUNT,
    };

    inline static const char *list[]{
        "EQUAL",
        "GREATER",
        "GREATER_EQUAL",
    };

    inline static const std::unordered_map<std::string, Type> map{};
};

template <>
class Ref<ref::lua::SKILL_COMPARE_FLAG> {
public:
    enum class Type {
        EQUAL,
        GREATER,
        GREATER_EQUAL,
        COUNT,
    };

    inline static const char *list[]{
        "EQUAL",
        "GREATER",
        "GREATER_EQUAL",
    };

    inline static const std::unordered_map<std::string, Type> map{};
};

template <>
class Ref<ref::lua::TARGET> {
public:
    enum class Type {
        NPC,
        PLAYER,
        COUNT,
    };

    inline static const char *list[]{
        "NPC",
        "PLAYER",
    };

    inline static const std::unordered_map<std::string, Type> map{};
};

template <>
class Ref<ref::lua::SKILL_KIND_TYPE> {
public:
    enum class Type {
        PHYSICS,
        SOLAR_MAGIC,
        LUNAR_MAGIC,
        NEUTRAL_MAGIC,
        POISON_MAGIC,
        COUNT,
    };

    inline static const char *list[]{
        "PHYSICS",
        "SOLAR_MAGIC",
        "LUNAR_MAGIC",
        "NEUTRAL_MAGIC",
        "POISON_MAGIC",
    };

    inline static const std::unordered_map<std::string, Type> map{};
};

template <>
class Ref<ref::lua::ROLE_TYPE> {
public:
    enum class Type {
        LITTLE_BOY,
        STANDARD_MALE,
        LITTLE_GIRL,
        STANDARD_FEMALE,
        COUNT,
    };

    inline static const char *list[]{
        "LITTLE_BOY",
        "STANDARD_MALE",
        "LITTLE_GIRL",
        "STANDARD_FEMALE",
    };

    inline static const std::unordered_map<std::string, Type> map{};
};

} // namespace frame
} // namespace jx3calc
