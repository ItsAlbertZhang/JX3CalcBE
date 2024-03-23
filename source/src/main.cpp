#include "frame/lua_static.h"
#include "gdi.h"
#include "modules/config.h"
#include "modules/web.h"
#ifdef _WIN32
#include <Windows.h>
#endif

int main(int argc, char *argv[]) {
    using namespace jx3calc;

    // 如果是 Windows 操作系统, 将控制台编码设置为 UTF-8, 以便输出中文.
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    // 初始化程序
    modules::config::init(argc, argv);
    // 初始化接口
    int ret = gdi::luaInit(frame::luaInit, frame::luaFuncList, frame::luaFuncListSize);
    if (0 != ret) {
        std::cerr << "Init failed." << std::endl;
#ifdef _WIN32
        system("pause");
#endif
        return 0;
    }

    modules::web::run();

    return 0;
}
