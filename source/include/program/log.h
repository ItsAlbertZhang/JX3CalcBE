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
    bool printImmediately = false;
    bool printLast = false;
    ~Log() {
        if (printLast)
            std::cout << data << std::endl;
    }
};

inline Log log_info{"info"};
inline Log log_error{"error"};

} // namespace ns_program

#ifdef _WIN32
#include <format>
using fmt = std;
#else
#include <fmt/core.h>
#endif

#define LOG_INFO(str, ...)                                                                   \
    {                                                                                        \
        std::string newdata = fmt::format("\n{}:{}: " str, __FILE__, __LINE__, __VA_ARGS__); \
        ns_program::log_info.data.append(newdata);                                           \
        if (ns_program::log_info.printImmediately)                                           \
            std::cout << newdata << std::endl;                                               \
    }

#define LOG_ERROR(str, ...)                                                                  \
    {                                                                                        \
        std::string newdata = fmt::format("\n{}:{}: " str, __FILE__, __LINE__, __VA_ARGS__); \
        ns_program::log_error.data.append(newdata);                                          \
        if (ns_program::log_error.printImmediately)                                          \
            std::cout << newdata << std::endl;                                               \
    }

#else // DEBUG

#define LOG_INFO(format, ...)
#define LOG_ERROR(format, ...)

#endif // DEBUG

#endif // PROGRAM_LOG_H_