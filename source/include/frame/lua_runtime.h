#ifndef FRAME_LUA_RUNTIME_H_
#define FRAME_LUA_RUNTIME_H_

#include <sol2/sol.hpp>
#include <string>
#include <unordered_map>

namespace ns_frame {

/**
 * @brief LuaFunc 类
 * @warning 这是一个静态类, 用于存放和管理 lua 中的函数, 不应当被创建实例.
 * @warning 这个类通过静态数据成员 thread_local 的形式实现线程安全.
 */
class LuaFunc {
public:
    LuaFunc() = delete; // 禁止创建类实例

    enum class Enum {
        GetSkillLevelData,
        GetSkillRecipeData,
        Apply,
        UnApply,
        OnRemove,
        OnTimer,
        COUNT, // 计数用
    };
    // 注意, 这个类接收 std::string &类型的参数, 均不保证 const 性. 它会将传入参数中的所有反斜杠替换为正斜杠.
    // 无需担心脏数据问题, 因为 global 下的 Manager::get() 返回均为 const 引用. 如果直接将其用于参数传入, 编译阶段就会报错.
    static int getIndex(std::string &filename, bool reload = false);
    static bool analysis(sol::protected_function_result res, std::string &filename, Enum func);
    static bool analysis(sol::protected_function_result res, int idx, Enum func);
    // 可以为获取函数重载参数类型 std::string filename / int idx, 但没必要, 暂时实现一个版本的就够了.
    static sol::protected_function getGetSkillLevelData(std::string &filename);
    static sol::protected_function getGetSkillRecipeData(std::string &filename);
    static sol::protected_function getApply(std::string &filename);
    static sol::protected_function getUnApply(std::string &filename);
    static sol::protected_function getOnRemove(std::string &filename);
    static sol::protected_function getOnTimer(int idx);
    /**
     * @note 在 MacOS 上, 程序退出时, 类的静态变量析构晚于在 gdi 库中的 lua 状态机(同样是类的静态变量)的析构.
     * @note 在 lua 状态机析构后, 再析构 filefuncList 中的 lua 函数, 会导致程序出口崩溃.
     * @note 因此, 暂时先使用此方法, 在程序退出时手动清空 filefuncList 中的 lua 函数.
     */
    static void clear();

private:
    static inline const std::string names[] = {
        "GetSkillLevelData",
        "GetSkillRecipeData",
        "Apply",
        "UnApply",
        "OnRemove",
        "OnTimer",
    };
    /**
     * @brief 缓存数据, 不同线程之间数据不共享
     */
    static inline thread_local std::vector<std::string> filenameList;
    static inline thread_local std::unordered_map<std::string, int> filenameMap;
    static inline thread_local std::vector<std::vector<sol::protected_function>> filefuncList;
    static const std::string &getFilename(int idx);
    static void add(const std::string &filename);
};

} // namespace ns_frame

#endif // FRAME_LUA_RUNTIME_H_