#ifdef DEBUG

#include "program/log.h"
#include <iostream>

namespace ns_program {
Log log_info{"info"};
Log log_error{"error"};
} // namespace ns_program
using namespace ns_program;

Log::Log(const std::string &name) : name(name) {
    data = new char[1024 * 1024];
    curr = data;
    memset(data, 0, 1024 * 1024);
}

Log::~Log() {
    std::cout << "\nLog (" << name << "):\n"
              << data << std::endl;
    delete[] data;
}

#endif // DEBUG