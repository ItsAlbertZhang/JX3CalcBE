#include "frame/common/globalparam.h"
#include "frame/lua/statics.h"
#include "gdi.h"
#include "modules/config.h"
#include "modules/web.h"
#ifdef _WIN32
#include <Windows.h>
#endif

int main(int argc, char *argv[]) {
    using namespace jx3calc;

    // 初始化数据
    bool ret = modules::config::init(argc, argv);
    if (ret) {
        // 初始化接口
        gdi::luaInit(
            frame::lua::statics::luaInit,
            frame::lua::statics::luaFuncList,
            frame::lua::statics::luaFuncListSize
        );
        // 初始化全局变量 (不需要每个线程都初始化一次, 因此不在 luaInit 中初始化)
        frame::GlobalParam::init();
    }

    // 运行 Web 服务器
    modules::web::run();

    return 0;
}
