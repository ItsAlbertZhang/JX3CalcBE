#pragma once

#include <string>
#include <unordered_map>

namespace jx3calc {
namespace frame {
namespace ref {

enum class enumSkillKindtype {
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
inline const std::unordered_map<std::string, enumSkillKindtype> mapSkillKindtype = {
    {"None",         enumSkillKindtype::None        },
    {"Physics",      enumSkillKindtype::Physics     },
    {"SolarMagic",   enumSkillKindtype::SolarMagic  },
    {"LunarMagic",   enumSkillKindtype::LunarMagic  },
    {"NeutralMagic", enumSkillKindtype::NeutralMagic},
    {"Poison",       enumSkillKindtype::Poison      },
    {"Leap",         enumSkillKindtype::Leap        },
    {"Adaptive",     enumSkillKindtype::Adaptive    },
};

} // namespace ref
} // namespace frame
} // namespace jx3calc
