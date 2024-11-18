#pragma once

#include <sol/sol.hpp>
#include <sol/state.hpp>
#include <string>

namespace jx3calc {
namespace frame {
namespace lua {

namespace interface {

sol::state *getLuaState();

enum class FuncType {
    GetSkillLevelData,
    GetSkillRecipeData,
    Apply,
    UnApply,
    OnRemove,
    OnTimer,
    ApplySetup,
    UnApplySetup,
    COUNT, // 计数用
};

// 注意, 以下所有函数接收 std::string &类型的参数, 均不保证 const 性. 它会将传入参数中的所有反斜杠替换为正斜杠.
// global 下的 Manager::get() 返回均为 const 引用. 如果直接将其用于参数传入, 编译阶段会报错.

int  getIndex(std::string &filename, bool reload = false);
bool analysis(sol::protected_function_result res, std::string &filename, FuncType func);
bool analysis(sol::protected_function_result res, int idx, FuncType func);

// 可以为获取函数重载参数类型 std::string filename / int idx, 但没必要, 暂时实现一个版本的就够了.
sol::protected_function getGetSkillLevelData(std::string &filename);
sol::protected_function getGetSkillRecipeData(std::string &filename);
sol::protected_function getApply(std::string &filename);
sol::protected_function getUnApply(std::string &filename);
sol::protected_function getOnRemove(std::string &filename);
sol::protected_function getOnTimer(int idx);
sol::protected_function getApplySetup(std::string &filename);
sol::protected_function getUnApplySetup(std::string &filename);

void include(const std::string &filename);

} // namespace interface

} // namespace lua
} // namespace frame
} // namespace jx3calc
