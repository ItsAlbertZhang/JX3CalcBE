#pragma once

#include <sol/sol.hpp>
#include <string>
#include <unordered_set>

namespace jx3calc {
namespace frame {
namespace lua {

namespace statics {

// lua 初始化函数
sol::state *luaInit();

// lua 函数表, 用于将点调用(静态调用)替换为冒号调用(动态调用)
extern const char *const luaFuncList[];
extern const size_t      luaFuncListSize;

// lua 文件黑名单, 用于处理已预编译为字节码的 lua 文件
extern const std::unordered_set<std::string> LuaBlacklistFiles;

} // namespace statics

} // namespace lua
} // namespace frame
} // namespace jx3calc
