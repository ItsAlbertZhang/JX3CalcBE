#pragma once

#include <string>
#include <unordered_map>

namespace jx3calc {
namespace frame {

struct ref {
    enum class Attrib;
    struct Skill {
        enum class KindType;
        enum class CastMode;
    };
    struct SkillEvent {
        enum class EventType;
        enum class CasterTarget;
    };
    struct lua {
        enum class ATTRIBUTE_TYPE;
        enum class KUNGFU_ADAPTIVETYPE_LIST;
        enum class MOVE_STATE;
        enum class ATTRIBUTE_EFFECT_MODE;
        enum class BUFF_COMPARE_FLAG;
        enum class SKILL_COMPARE_FLAG;
        enum class TARGET;
        enum class SKILL_KIND_TYPE;
        enum class ROLE_TYPE;
        enum class ABSORB_ATTRIBUTE_SHIELD_TYPE;
        enum class PLAYER_ARENA_TYPE;
    };
};

template <typename T>
    requires std::is_enum_v<T>
class Ref {
    template <auto value>
    static constexpr auto enum_name() {
        std::string_view name;
#if __GNUC__ || __clang__
        name              = __PRETTY_FUNCTION__;
        std::size_t start = name.find('=') + 2;
        std::size_t end   = name.size() - 1;
        name              = std::string_view {name.data() + start, end - start};
        start             = name.rfind("::");
#elif _MSC_VER
        name              = __FUNCSIG__;
        std::size_t start = name.find('<') + 1;
        std::size_t end   = name.rfind(">(");
        name              = std::string_view {name.data() + start, end - start};
        start             = name.rfind("::");
#endif
        return start == std::string_view::npos ? name : std::string_view {name.data() + start + 2, name.size() - start - 2};
    }

    template <std::size_t N = 0>
    static constexpr auto enum_max() {
        constexpr auto value = static_cast<T>(N);
        if constexpr (enum_name<value>().find(")") == std::string_view::npos)
            return enum_max<N + 1>();
        else
            return N;
    }

public:
    inline static constexpr std::size_t count = enum_max<>();

private:
    static constexpr auto gen_name() {
        constexpr auto names = []<std::size_t... Is>(std::index_sequence<Is...>) {
            return std::array<std::string_view, count> {
                enum_name<static_cast<T>(Is)>()...
            };
        }(std::make_index_sequence<count> {});
        return names;
    }

public:
    inline static constexpr std::array<std::string_view, count> names = gen_name();

private:
    static const std::unordered_map<std::string, T> gen_map() {
        return []<std::size_t... Is>(std::index_sequence<Is...>) {
            return std::unordered_map<std::string, T> {
                {std::string {names[Is]}, static_cast<T>(Is)}...
            };
        }(std::make_index_sequence<count> {});
    }

public:
    inline static const std::unordered_map<std::string, T> map = gen_map();
};

} // namespace frame
} // namespace jx3calc
