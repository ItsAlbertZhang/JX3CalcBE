#ifndef PROGRAM_LOG_H_
#define PROGRAM_LOG_H_

#ifdef DEBUG

#include <string>

namespace ns_program {

class Log {
public:
    Log(const std::string &name);
    ~Log();
    template <typename... Args>
    void operator()(const char *format, Args... args) {
        curr += snprintf(curr, 1024 * 1024 - (curr - data), format, args...);
    }

private:
    std::string name;
    char *curr = nullptr;
    char *data = nullptr;
};

extern Log log_info;
extern Log log_error;

} // namespace ns_program

#define LOG_INFO(format, ...) \
    ns_program::log_info(format, __VA_ARGS__);

#define LOG_ERROR(format, ...) \
    ns_program::log_error("%s:%d: " format, __FILE__, __LINE__, __VA_ARGS__);

#else

#define LOG_INFO(format, ...)
#define LOG_ERROR(format, ...)

#endif // DEBUG

#endif // PROGRAM_LOG_H_