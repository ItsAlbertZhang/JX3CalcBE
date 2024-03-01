#ifndef GLOBAL_CONSTEXPR_LOG_H_
#define GLOBAL_CONSTEXPR_LOG_H_

#ifdef D_CONSTEXPR_LOG

#include <string>

#ifdef _WIN32
#include <format>
namespace fmt = std;
#else
#include <fmt/core.h>
#endif

namespace ns_plugin {
namespace log {

using format_args = fmt::format_args; // 消除编译器对于 #include <format> 和 namespace fmt = std 未使用的警告

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
} // namespace ns_plugin

#define CONSTEXPR_LOG_INFO(str, ...)  ns_plugin::log::info(fmt::format("{}:{}: " str "\n", __FILE__, __LINE__, __VA_ARGS__));
#define CONSTEXPR_LOG_ERROR(str, ...) ns_plugin::log::error(fmt::format("{}:{}: " str "\n", __FILE__, __LINE__, __VA_ARGS__));

#else // D_CONSTEXPR_LOG

#define CONSTEXPR_LOG_INFO(str, ...)
#define CONSTEXPR_LOG_ERROR(str, ...)

#endif // D_CONSTEXPR_LOG

#endif // GLOBAL_CONSTEXPR_LOG_H_
