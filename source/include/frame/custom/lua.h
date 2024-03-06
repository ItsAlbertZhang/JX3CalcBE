#ifndef FRAME_CUSTOM_LUA_H_
#define FRAME_CUSTOM_LUA_H_

#include <memory>
#include <sol/sol.hpp>
#include <unordered_map>
namespace ns_frame {

class CustomLua {
public:
    CustomLua(const std::string &script);

    sol::state                           lua; // lua 状态机
    sol::protected_function              macroPrepare;
    std::vector<sol::protected_function> macroRuntime;
};
inline thread_local std::unordered_map<std::string, std::unique_ptr<CustomLua>> mapCustomLua;

} // namespace ns_frame

#endif
