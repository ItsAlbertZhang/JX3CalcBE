#ifndef FRAME_REF_LUA_ADAPTIVE_TYPE_H_
#define FRAME_REF_LUA_ADAPTIVE_TYPE_H_

#include <unordered_map>

namespace jx3calc {
namespace frame {
namespace ref {

enum class enumLuaAdaptiveType {
    physics, // 在 lua 中出现在最后面, 但是 lua 是从 1 开始的
    lunar,
    solar,
    neutral,
    poison,
    COUNT,
};
enum class enumLuaAdaptiveKungfuID {
    mj_fysj = 10242,
    COUNT,
};

inline const std::unordered_map<enumLuaAdaptiveKungfuID, enumLuaAdaptiveType> mapLuaAdaptiveType = {
    {enumLuaAdaptiveKungfuID::mj_fysj, enumLuaAdaptiveType::lunar}
};
} // namespace ref
} // namespace frame
} // namespace jx3calc

#endif // FRAME_REF_LUA_ADAPTIVE_TYPE_H_
