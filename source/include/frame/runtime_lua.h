#ifndef FRAME_RUNTIME_LUA_H_
#define FRAME_RUNTIME_LUA_H_

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
        Apply,
        OnTimer,
        COUNT, // 计数用
    };
    // 注意, 这个类接收 std::string &类型的参数, 均不保证 const 性. 它会将传入参数中的所有反斜杠替换为正斜杠.
    static int getIndex(std::string &filename);
    static sol::protected_function getGetSkillLevelData(std::string &filename);
    static sol::protected_function getApply(std::string &filename);
    static sol::protected_function getOnTimer(int idx);
    static bool analysis(sol::protected_function_result res, std::string &filename, Enum func);
    static bool analysis(sol::protected_function_result res, int idx, Enum func);

private:
    static inline const std::string names[] = {
        "GetSkillLevelData",
        "Apply",
        "OnTimer",
    };
    /**
     * @brief 缓存数据, 不同线程之间数据不共享
     * @note filename 存在有无法区分正反斜杠的问题, 但不影响使用.
     * @note 在遇到不同的斜杠时, lua 会被再加载一次, 这样做的效率应该要高于每次检查传入并进行替换.
     */
    static inline thread_local std::vector<std::string> filenameList;
    static inline thread_local std::unordered_map<std::string, int> filenameMap;
    static inline thread_local std::vector<std::vector<sol::protected_function>> filefuncList;
    static const std::string &getFilename(int idx);
    static void add(const std::string &filename);
};

} // namespace ns_frame

#endif // FRAME_RUNTIME_LUA_H_