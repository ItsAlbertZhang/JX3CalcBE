#include "frame/common/globalparam.h"
#include "gdi.h"
#include "plugin/log.h"

using namespace jx3calc::frame;

int GlobalParam::levelCof(int level) {
    if (level > 120)
        return 1155 * (level - 120) + 8250;
    else if (level > 110)
        return 450 * (level - 110) + 3750;
    else if (level > 100)
        return 205 * (level - 100) + 1700;
    else if (level > 95)
        return 185 * (level - 95) + 775;
    else if (level > 90)
        return 85 * (level - 90) + 350;
    else if (level > 15)
        return 4 * level - 10;
    else
        return 50;
}

bool GlobalParam::init() {
    gdi::luaExecuteFile("scripts/skill/GlobalParam.lua");
    auto res = gdi::luaGetFunction("Load")();
    if (!res.valid()) {
        sol::error err = res;
        CONSTEXPR_LOG_ERROR("GlobalParam init failed: \n{}", err.what());
        return false;
    } else {
        CONSTEXPR_LOG_INFO("GlobalParam init success.{}", "");
        return true;
    }
}
