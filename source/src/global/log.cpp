#include "global/log.h"
#include "global/config.h"
#include <fstream>
#include <iostream>

using namespace nsg_log;

#ifdef DEBUG

void Log::operator()(const std::string &newdata) {
    if (enable)
        data.append(newdata);
    if (output)
        std::cout << newdata << std::endl;
}

void Log::save() {
    std::ofstream file(nsg_config::pExeDir / ("log_" + name + ".log"));
    file << data;
    file.close();
}

#endif // DEBUG
