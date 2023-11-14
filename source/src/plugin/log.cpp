#ifdef D_CONSTEXPR_LOG

#include "plugin/log.h"
#include "modules/config.h"
#include <fstream>
#include <iostream>

using namespace jx3calc;
using namespace plugin::log;

void Log::operator()(const std::string &newdata) {
    if (enable) {
        data.append(newdata);
        if (output)
            std::cout << newdata << std::endl;
    }
}

void Log::save() {
    std::ofstream file(modules::config::pExeDir / ("log_" + name + ".tab"));
    file << data;
    file.close();
}

#endif // D_CONSTEXPR_LOG
