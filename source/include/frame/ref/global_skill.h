#ifndef FRAME_REF_GLOBAL_SKILL_H_
#define FRAME_REF_GLOBAL_SKILL_H_

#include <string>
#include <unordered_map>

namespace ns_frame {
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
} // namespace ns_frame
#endif // FRAME_REF_GLOBAL_SKILL_H_
