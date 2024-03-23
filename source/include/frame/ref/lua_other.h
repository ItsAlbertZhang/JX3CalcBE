#ifndef FRAME_REF_LUA_OTHER_H_
#define FRAME_REF_LUA_OTHER_H_

namespace jx3calc {
namespace frame {
namespace ref {

enum class enumLuaAbsorbAttributeShieldType {
    GLOBAL,
    PHYSICS,
    SOLAR,
    NEUTRAL,
    LUNAR,
    POISON,
    COUNT,
};
inline const char *refLuaAbsorbAttributeShieldType[] = {
    "GLOBAL",
    "PHYSICS",
    "SOLAR",
    "NEUTRAL",
    "LUNAR",
    "POISON",
};

enum class enumLuaAttributeEffectMode {
    EFFECT_TO_SELF_NOT_ROLLBACK,
    EFFECT_TO_SELF_AND_ROLLBACK,
    EFFECT_TO_DEST_NOT_ROLLBACK,
    EFFECT_TO_DEST_AND_ROLLBACK,
    COUNT,
};
inline const char *refLuaAttributeEffectMode[] = {
    "EFFECT_TO_SELF_NOT_ROLLBACK",
    "EFFECT_TO_SELF_AND_ROLLBACK",
    "EFFECT_TO_DEST_NOT_ROLLBACK",
    "EFFECT_TO_DEST_AND_ROLLBACK",
};

enum class enumLuaBuffCompareFlag {
    EQUAL,
    GREATER,
    GREATER_EQUAL,
    COUNT,
};
inline const char *refLuaBuffCompareFlag[] = {
    "EQUAL",
    "GREATER",
    "GREATER_EQUAL",
};

enum class enumLuaSkillCompareFlag {
    EQUAL,
    GREATER,
    GREATER_EQUAL,
    COUNT,
};
inline const char *refLuaSkillCompareFlag[] = {
    "EQUAL",
    "GREATER",
    "GREATER_EQUAL",
};

enum class enumLuaTarget {
    NPC,
    PLAYER,
    COUNT,
};
inline const char *refLuaTarget[] = {
    "NPC",
    "PLAYER",
};

enum class enumLuaSkillKindType {
    PHYSICS,
    SOLAR_MAGIC,
    LUNAR_MAGIC,
    NEUTRAL_MAGIC,
    POISON_MAGIC,
    COUNT,
};
inline const char *refLuaSkillKindType[] = {
    "PHYSICS",
    "SOLAR_MAGIC",
    "LUNAR_MAGIC",
    "NEUTRAL_MAGIC",
    "POISON_MAGIC",
};

enum class enumLuaRoleType {
    LITTLE_BOY,
    STANDARD_MALE,
    LITTLE_GIRL,
    STANDARD_FEMALE,
    COUNT,
};
inline const char *refLuaRoleType[] = {
    "LITTLE_BOY",
    "STANDARD_MALE",
    "LITTLE_GIRL",
    "STANDARD_FEMALE",
};

// 几乎确定不全, 出现于 明教/明教_暗尘弥散.lua
enum class enumLuaMoveState {
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
inline const char *refLuaMoveState[] = {
    "ON_ENTRAP",
    "ON_FREEZE",
    "ON_HALT",
    "ON_KNOCKED_BACK",
    "ON_KNOCKED_DOWN",
    "ON_KNOCKED_OFF",
    "ON_SKILL_MOVE_DST",
    "ON_REPULSED",
};

} // namespace ref
} // namespace frame
} // namespace jx3calc

#endif // FRAME_REF_LUA_OTHER_H_
