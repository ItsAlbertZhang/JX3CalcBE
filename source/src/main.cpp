#include "frame/lua_static.h"
#include "gdi.h"
#include "modules/web.h"
#include "utils/config.h"
#ifdef _WIN32
#include <Windows.h>
#endif

int main(int argc, char *argv[]) {

    // 如果是 Windows 操作系统, 将控制台编码设置为 UTF-8, 以便输出中文.
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    // 初始化程序
    ns_utils::config::init(argc, argv);
    // 初始化接口
    int ret = gdi::luaInit(ns_frame::luaInit, ns_frame::luaFuncList, ns_frame::luaFuncListSize);
    if (0 != ret) {
        std::cerr << "Init failed." << std::endl;
#ifdef _WIN32
        system("pause");
#endif
        return 0;
    }

    ns_modules::web::run();

    return 0;
}
