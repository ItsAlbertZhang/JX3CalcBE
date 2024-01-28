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

namespace nsgc_log {

using format_args = fmt::format_args; // 消除编译器对于 #include <format> 和 namespace fmt = std 未使用的警告

class Log {
public:
    Log(const std::string &name)
        : name(name) {}

    std::string name;
    std::string data;
    bool        enable = false;
    bool        output = false;

    void operator()(const std::string &newdata);
    void save();
};

// 日志对顺序有要求, 因此使用 thread_local
inline thread_local Log info{"info"};
inline thread_local Log error{"error"};

} // namespace nsgc_log

#define CONSTEXPR_LOG_INFO(str, ...)  nsgc_log::info(fmt::format("{}:{}: " str "\n", __FILE__, __LINE__, __VA_ARGS__));
#define CONSTEXPR_LOG_ERROR(str, ...) nsgc_log::error(fmt::format("{}:{}: " str "\n", __FILE__, __LINE__, __VA_ARGS__));

#else // D_CONSTEXPR_LOG

#define CONSTEXPR_LOG_INFO(str, ...)
#define CONSTEXPR_LOG_ERROR(str, ...)

#endif // D_CONSTEXPR_LOG

#endif // GLOBAL_CONSTEXPR_LOG_H_
