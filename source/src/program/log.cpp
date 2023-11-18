#include "program/log.h"
#include "iostream"

using namespace ns_program;

Log &ns_program::log = Log::getInstance();

Log &Log::getInstance() {
    static Log instance;
    return instance;
}

Log::Log() {
    data = new char[1024 * 1024];
    curr = data;
    memset(data, 0, 1024 * 1024);
}

Log::~Log() {
    std::cout << "\nLog: (Page " << page++ << ")\n\n"
              << data << std::endl;
    delete[] data;
}

void Log::operator()(const std::string &format) {
    if (format.length() > static_cast<size_t>(data + 1024 * 1024 - curr)) {
        std::cout << "\nLog: (Page " << page++ << ")\n\n"
                  << data << std::endl;
        curr = data;
        memset(data, 0, 1024 * 1024);
    }
    strncpy(curr, format.c_str(), format.length());
    curr += format.length();
}