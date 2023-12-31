#include "frame/lua_static.h"
#include "gdi.h"
#include "global/settings.h"
#include "program/thread_web.h"
#ifdef _WIN32
#include <Windows.h>
#endif

int main(int argc, char *argv[]) {

    // 如果是 Windows 操作系统, 将控制台编码设置为 UTF-8, 以便输出中文.
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    bool ret;
    // 初始化程序
    ns_global::init(argc, argv);
    // 初始化接口
    ret = gdi::init(
        ns_global::Config::pJX3,
        ns_global::Config::pUnpack,
        ns_frame::luaInit,
        ns_frame::luaFuncStaticToDynamic,
        static_cast<int>(gdi::Tab::COUNT)
    );
    if (!ret)
        return 0;

    ns_program::Web webHandler;
    std::string     s;
    while (std::getline(std::cin, s)) {
        if (s == "exit")
            break;
    }
    webHandler.stop();

    return 0;
}
