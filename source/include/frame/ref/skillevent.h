#pragma once

#include "frame/ref/ref.h"

namespace jx3calc {
namespace frame {

enum class ref::SkillEvent::EventType {
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
};

enum class ref::SkillEvent::CasterTarget {
    EventCaster,
    EventTarget,
};

} // namespace frame
} // namespace jx3calc
