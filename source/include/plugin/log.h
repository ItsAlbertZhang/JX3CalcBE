#ifndef GLOBAL_CONSTEXPR_LOG_H_
#define GLOBAL_CONSTEXPR_LOG_H_

#ifdef D_CONSTEXPR_LOG

#include <format>
#include <string>

namespace jx3calc {
namespace plugin {
namespace log {

using format_args = std::format_args; // 消除编译器对于 #include <format> 和 namespace std = std 未使用的警告

class Log {
public:
    std::string name;
    std::string data;
    bool        enable = true;
    bool        output = false;

    void operator()(const std::string &newdata);
    void save();
};

inline Log info{.name = "info"};
inline Log error{.name = "error", .output = true};

} // namespace log
} // namespace plugin
} // namespace jx3calc

#define CONSTEXPR_LOG_INFO(str, ...)  plugin::log::info(std::format("{}:{}: " str "\n", __FILE__, __LINE__, __VA_ARGS__));
#define CONSTEXPR_LOG_ERROR(str, ...) plugin::log::error(std::format("{}:{}: " str "\n", __FILE__, __LINE__, __VA_ARGS__));

#else // D_CONSTEXPR_LOG

#define CONSTEXPR_LOG_INFO(str, ...) \
    do {                             \
    } while (0)
#define CONSTEXPR_LOG_ERROR(str, ...) \
    do {                              \
    } while (0)
// do...while 的目的是在不影响性能的同时, 保证语句非置空, 以消除 MSVC 警告: “;”: 找到空的受控语句；这是否是有意的?

#endif // D_CONSTEXPR_LOG

#endif // GLOBAL_CONSTEXPR_LOG_H_
