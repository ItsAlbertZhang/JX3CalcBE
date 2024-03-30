#pragma once

#include "frame/ref/ref.h"

namespace jx3calc {
namespace frame {

enum class ref::lua::KUNGFU_ADAPTIVETYPE_LIST {};

template <> // 模板特化
class Ref<ref::lua::KUNGFU_ADAPTIVETYPE_LIST> {
public:
    enum class type {
        none,
        lunar,
        solar,
        neutral,
        poison,
        physics,
    };

    enum class kungfu {
        MjFysj = 10242,
    };

    inline static const std::unordered_map<kungfu, type> kungfu2type = {
        {kungfu::MjFysj, type::lunar},
    };
};

// 几乎肯定不全, 出现于 明教/明教_暗尘弥散.lua
enum class ref::lua::MOVE_STATE {
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

} // namespace frame
} // namespace jx3calc
