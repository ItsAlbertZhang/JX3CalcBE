#ifndef FRAME_STATICS_LUA_BLACKLIST_FILES_H_
#define FRAME_STATICS_LUA_BLACKLIST_FILES_H_

#include <string>
#include <unordered_set>

namespace jx3calc {
namespace frame {

// 预编译的 lua 文件
inline const std::unordered_set<std::string> staticsLuaBlacklistFiles = {
    "scripts/player/include/Kungfu2ArenaType.lh",
};

} // namespace frame
} // namespace jx3calc

#endif // FRAME_STATICS_LUA_BLACKLIST_FILES_H_
