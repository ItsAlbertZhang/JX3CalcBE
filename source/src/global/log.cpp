#include "global/log.h"
#include "global/settings.h"
#include <fstream>
#include <iostream>

using namespace ns_global;

#ifdef DEBUG

void Log::operator()(const std::string &newdata) {
    if (enable)
        data.append(newdata);
    if (output)
        std::cout << newdata << std::endl;
}

void Log::save() {
    std::ofstream file(Config::pExeDir / ("log_" + name + ".log"));
    file << data;
    file.close();
}

#endif // DEBUG
