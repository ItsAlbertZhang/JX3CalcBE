#pragma once

#include <sol/sol.hpp>

namespace jx3calc {
namespace frame {

class CustomLua {
public:
    static inline thread_local std::unordered_map<std::string, std::shared_ptr<CustomLua>> mapCustomLua;

    static std::shared_ptr<CustomLua> get(const std::string &script);
    static void                       cancel(const std::string &script);
    static std::string                parse(std::vector<std::string> macroList);

    CustomLua(const std::string &script);

    sol::state                           lua; // lua 状态机
    sol::protected_function              fightPrepareAdd;
    std::vector<sol::protected_function> macroRuntime;
};

} // namespace frame
} // namespace jx3calc
