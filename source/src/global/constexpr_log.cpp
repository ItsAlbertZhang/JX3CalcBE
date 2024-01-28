#ifdef D_CONSTEXPR_LOG

#include "global/constexpr_log.h"
#include "global/config.h"
#include <fstream>
#include <iostream>

using namespace nsgc_log;

void Log::operator()(const std::string &newdata) {
    if (enable)
        data.append(newdata);
    if (output)
        std::cout << newdata << std::endl;
}

void Log::save() {
    std::ofstream file(nsg_config::pExeDir / ("log_" + name + ".tab"));
    file << data;
    file.close();
}

#endif // D_CONSTEXPR_LOG
