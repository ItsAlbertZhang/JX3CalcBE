#pragma once

#include "frame/ref/ref.h"

namespace jx3calc {
namespace frame {

template <>
class Ref<ref::SkillEvent::EventType> {
public:
    enum class Type {
        BeCast,
        BeCriticalStrike,
        BeDodge,
        BeHit,
        BeHitOTAction,
        BeKill,
        BeMiss,
        BeOverHeal,
        BlockLongRange,
        Cast,
        CriticalStrike,
        Dodge,
        Hit,
        HitOTAction,
        Kill,
        Miss,
        OverHeal,
        Parry,
        PreCast,
        COUNT,
    };

    inline static const char *list[]{};

    inline static const std::unordered_map<std::string, Type> map{
        {"BeCast",           Type::BeCast          },
        {"BeCriticalStrike", Type::BeCriticalStrike},
        {"BeDodge",          Type::BeDodge         },
        {"BeHit",            Type::BeHit           },
        {"BeHitOTAction",    Type::BeHitOTAction   },
        {"BeKill",           Type::BeKill          },
        {"BeMiss",           Type::BeMiss          },
        {"BeOverHeal",       Type::BeOverHeal      },
        {"BlockLongRange",   Type::BlockLongRange  },
        {"Cast",             Type::Cast            },
        {"CriticalStrike",   Type::CriticalStrike  },
        {"Dodge",            Type::Dodge           },
        {"Hit",              Type::Hit             },
        {"HitOTAction",      Type::HitOTAction     },
        {"Kill",             Type::Kill            },
        {"Miss",             Type::Miss            },
        {"OverHeal",         Type::OverHeal        },
        {"Parry",            Type::Parry           },
        {"PreCast",          Type::PreCast         },
    };
};

template <>
class Ref<ref::SkillEvent::CasterTarget> {
public:
    enum class Type {
        EventCaster,
        EventTarget,
        COUNT,
    };

    inline static const char *list[]{};

    inline static const std::unordered_map<std::string, Type> map{
        {"EventCaster", Type::EventCaster},
        {"EventTarget", Type::EventTarget},
    };
};

} // namespace frame
} // namespace jx3calc
