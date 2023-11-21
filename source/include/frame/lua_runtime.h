#ifndef FRAME_LUA_FUNC_H_
#define FRAME_LUA_FUNC_H_

#include <sol2/sol.hpp>
#include <string>
#include <unordered_map>

namespace ns_frame {

class LuaFunc {
public:
    LuaFunc() = delete; // 禁止创建类实例
    static sol::protected_function getApply(std::string filename);

private:
    enum class Enum {
        Apply,
        COUNT, // 计数用
    };
    static inline const std::string names[] = {
        "Apply",
    };
    /**
     * @brief 缓存数据, 不同线程之间数据不共享
     * @note key 为 filename, value 为一个 vector, 其内存储该文件内的所有函数.
     * @note vector 的 size() 应当等于 static_cast<int>(Enum::COUNT)
     */
    static inline thread_local std::unordered_map<std::string, std::vector<sol::protected_function>> data;
    static void add(std::string filename);
};

} // namespace ns_frame

#endif // FRAME_LUA_FUNC_H_