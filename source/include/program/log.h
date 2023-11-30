#ifndef PROGRAM_LOG_H_
#define PROGRAM_LOG_H_

#ifdef DEBUG

#include <iostream>
#include <string>

namespace ns_program {

class Log {
public:
    Log(const std::string &name) : name(name) {
        data = new char[size];
        curr = data;
        memset(data, 0, size);
    }

    ~Log() {
        delete[] data;
    }

    template <typename... Args>
    void operator()(const char *format, Args... args) {
        if (static_cast<size_t>(curr - data + 1024) > size) {
            char *tmp = new char[size * 2];
            memcpy(tmp, data, size);
            delete[] data;
            curr = tmp + (curr - data);
            data = tmp;
            size *= 2;
        }
        curr += snprintf(curr, size - (curr - data), format, args...);
    }

    void print() {
        std::cout << "\nLog (" << name << "):\n"
                  << data << std::endl;
    }

private:
    std::string name;
    size_t size = 1024 * 1024;
    char *curr = nullptr;
    char *data = nullptr;
};

inline Log log_info{"info"};
inline Log log_error{"error"};

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