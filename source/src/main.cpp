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
    ns_utils::config::initExeDir(argc, argv);
    // 尝试从配置文件中初始化数据
    ns_utils::config::initDataFromLocalFile();
    // 初始化接口
    bool ret = gdi::initLua(ns_frame::luaInit, ns_frame::luaFuncStaticToDynamic);
    if (!ret) {
        std::cerr << "Init failed." << std::endl;
#ifdef _WIN32
        system("pause");
#endif
        return 0;
    }

    ns_modules::web::run();

    return 0;
}
