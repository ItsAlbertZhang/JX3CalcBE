#ifndef FRAME_LUA_FUNC_H_
#define FRAME_LUA_FUNC_H_

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
    static int getIndex(const std::string &filename);
    static sol::protected_function getApply(const std::string &filename);
    static sol::protected_function getOnTimer(int idx);

private:
    enum class Enum {
        Apply,
        OnTimer,
        COUNT, // 计数用
    };
    static inline const std::string names[] = {
        "Apply",
        "OnTimer",
    };
    /**
     * @brief 缓存数据, 不同线程之间数据不共享
     * @note key 为 filename, value 为一个 vector, 其内存储该文件内的所有函数.
     * @note vector 的 size() 应当等于 static_cast<int>(Enum::COUNT)
     */
    static inline thread_local std::unordered_map<std::string, int> filenameMap;
    static inline thread_local std::vector<std::vector<sol::protected_function>> filefuncList;
    static void add(const std::string &filename);
};

} // namespace ns_frame

#endif // FRAME_LUA_FUNC_H_