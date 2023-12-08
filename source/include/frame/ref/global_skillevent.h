#ifndef FRAME_REF_GLOBAL_SKILLEVENT_H_
#define FRAME_REF_GLOBAL_SKILLEVENT_H_

#include <string>
#include <unordered_map>

namespace ns_frame {
namespace ref {

enum class enumSkilleventEventtype {
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
inline const std::unordered_map<std::string, enumSkilleventEventtype> mapSkilleventEventtype = {
    {"BeCast", enumSkilleventEventtype::BeCast},
    {"BeCriticalStrike", enumSkilleventEventtype::BeCriticalStrike},
    {"BeDodge", enumSkilleventEventtype::BeDodge},
    {"BeHit", enumSkilleventEventtype::BeHit},
    {"BeHitOTAction", enumSkilleventEventtype::BeHitOTAction},
    {"BeKill", enumSkilleventEventtype::BeKill},
    {"BeMiss", enumSkilleventEventtype::BeMiss},
    {"BeOverHeal", enumSkilleventEventtype::BeOverHeal},
    {"BlockLongRange", enumSkilleventEventtype::BlockLongRange},
    {"Cast", enumSkilleventEventtype::Cast},
    {"CriticalStrike", enumSkilleventEventtype::CriticalStrike},
    {"Dodge", enumSkilleventEventtype::Dodge},
    {"Hit", enumSkilleventEventtype::Hit},
    {"HitOTAction", enumSkilleventEventtype::HitOTAction},
    {"Kill", enumSkilleventEventtype::Kill},
    {"Miss", enumSkilleventEventtype::Miss},
    {"OverHeal", enumSkilleventEventtype::OverHeal},
    {"Parry", enumSkilleventEventtype::Parry},
    {"PreCast", enumSkilleventEventtype::PreCast},
};

enum class enumSkilleventCastertarget {
    EventCaster,
    EventTarget,
    COUNT,
};
inline const std::unordered_map<std::string, enumSkilleventCastertarget> mapSkilleventCastertarget = {
    {"EventCaster", enumSkilleventCastertarget::EventCaster},
    {"EventTarget", enumSkilleventCastertarget::EventTarget},
};

} // namespace ref
} // namespace ns_frame
#endif // FRAME_REF_GLOBAL_SKILLEVENT_H_