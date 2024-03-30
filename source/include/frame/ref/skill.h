#pragma once

#include "frame/ref/ref.h"

namespace jx3calc {
namespace frame {

template <>
class Ref<ref::Skill::KindType> {
public:
    enum class Type {
        None,
        Physics,
        SolarMagic,
        LunarMagic,
        NeutralMagic,
        Poison,
        Leap,
        Adaptive,
        COUNT,
    };

    inline static const char *list[]{};

    inline static const std::unordered_map<std::string, Type> map{
        {"None",         Type::None        },
        {"Physics",      Type::Physics     },
        {"SolarMagic",   Type::SolarMagic  },
        {"LunarMagic",   Type::LunarMagic  },
        {"NeutralMagic", Type::NeutralMagic},
        {"Poison",       Type::Poison      },
        {"Leap",         Type::Leap        },
        {"Adaptive",     Type::Adaptive    },
    };
};

} // namespace frame
} // namespace jx3calc
