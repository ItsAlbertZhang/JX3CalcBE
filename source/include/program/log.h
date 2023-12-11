#ifndef PROGRAM_LOG_H_
#define PROGRAM_LOG_H_

#ifdef DEBUG

#include <iostream>
#include <string>

namespace ns_program {

class Log {
public:
    Log(const std::string &name) : name(name) {}
    std::string name;
    std::string data;
    bool record = false;
    bool realtime = false;
    void print() {
        std::cout << data << std::endl;
        data.clear();
    }
};

inline Log log_info{"info"};
inline Log log_error{"error"};

} // namespace ns_program

#ifdef _WIN32
#include <format>
namespace fmt = std;
#else
#include <fmt/core.h>
#endif

#define LOG_INFO(str, ...)                                                                   \
    {                                                                                        \
        std::string newdata = fmt::format("\n{}:{}: " str, __FILE__, __LINE__, __VA_ARGS__); \
        if (ns_program::log_info.record)                                                     \
            ns_program::log_info.data.append(newdata);                                       \
        if (ns_program::log_info.realtime)                                                   \
            std::cout << newdata << std::endl;                                               \
    }

#define LOG_ERROR(str, ...)                                                                  \
    {                                                                                        \
        std::string newdata = fmt::format("\n{}:{}: " str, __FILE__, __LINE__, __VA_ARGS__); \
        if (ns_program::log_info.record)                                                     \
            ns_program::log_error.data.append(newdata);                                      \
        if (ns_program::log_error.realtime)                                                  \
            std::cout << newdata << std::endl;                                               \
    }

#else // DEBUG

#define LOG_INFO(format, ...)
#define LOG_ERROR(format, ...)

#endif // DEBUG

#endif // PROGRAM_LOG_H_