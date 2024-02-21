#ifdef D_CONSTEXPR_LOG

#include "plugin/log.h"
#include "utils/config.h"
#include <fstream>
#include <iostream>

using namespace ns_plugin::log;

void Log::operator()(const std::string &newdata) {
    if (enable) {
        data.append(newdata);
        if (output)
            std::cout << newdata << std::endl;
    }
}

void Log::save() {
    std::ofstream file(ns_utils::config::pExeDir / ("log_" + name + ".tab"));
    file << data;
    file.close();
    enable = false;
}

#endif // D_CONSTEXPR_LOG
