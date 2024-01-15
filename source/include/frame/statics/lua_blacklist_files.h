#ifndef FRAME_STATICS_LUA_BLACKLIST_FILES_H_
#define FRAME_STATICS_LUA_BLACKLIST_FILES_H_

#include <string>
#include <unordered_set>
namespace ns_frame {

// 预编译的 lua 文件
inline const std::unordered_set<std::string> staticsLuaBlacklistFiles = {
    "scripts/player/include/Kungfu2ArenaType.lh",
};

} // namespace ns_frame

#endif // FRAME_STATICS_LUA_BLACKLIST_FILES_H_
