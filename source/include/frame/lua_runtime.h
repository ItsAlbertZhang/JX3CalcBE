#ifndef FRAME_LUA_FUNC_H_
#define FRAME_LUA_FUNC_H_

#include <sol2/sol.hpp>
#include <string>
#include <unordered_map>

namespace ns_frame {

class LuaApply {
public:
    LuaApply() = delete; // 禁止创建类实例
    static sol::protected_function get(std::string);

private:
    static inline thread_local std::unordered_map<std::string, sol::protected_function> data; // 数据存放区, 不同线程之间数据不共享
    static void add(std::string);
};

} // namespace ns_frame

#endif // FRAME_LUA_FUNC_H_