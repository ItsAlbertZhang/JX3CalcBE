#ifndef FRAME_STATIC_REF_H_
#define FRAME_STATIC_REF_H_

namespace ns_framestatic {

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

} // namespace ns_framestatic

#endif // FRAME_STATIC_REF_H_