#ifndef GLOBAL_LOG_H_
#define GLOBAL_LOG_H_

#ifdef DEBUG

#include <string>

#ifdef _WIN32
#include <format>
namespace fmt = std;
#else
#include <fmt/core.h>
#endif

namespace ns_global {

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

inline Log log_info{"info"};
inline Log log_error{"error"};

} // namespace ns_global

#define LOG_INFO(str, ...)  ns_global::log_info(fmt::format("{}:{}: " str "\n", __FILE__, __LINE__, __VA_ARGS__));
#define LOG_ERROR(str, ...) ns_global::log_error(fmt::format("{}:{}: " str "\n", __FILE__, __LINE__, __VA_ARGS__));

#else // DEBUG

#define LOG_INFO(str, ...)
#define LOG_ERROR(str, ...)

#endif // DEBUG

#endif // GLOBAL_LOG_H_
