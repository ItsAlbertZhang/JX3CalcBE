#include "frame/lua_static.h"
#include "gdi.h"
#include "global/config.h"
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
    nsg_config::init(argc, argv);
    // 初始化接口
    ret = gdi::init(
        nsg_config::pJX3,
        nsg_config::pUnpack,
        ns_frame::luaInit,
        ns_frame::luaFuncStaticToDynamic,
        static_cast<int>(gdi::Tab::COUNT)
    );
    if (!ret) {
        std::cout << "Error: 初始化失败! 请检查 config.json 中的游戏路径是否正确, 尤其检查正反斜杠问题 (路径的分隔使用正斜杠 '/' 或双反斜杠 '\\\\')." << std::endl;
        std::cout << "Error: Init failed! Check config.json, especially the \"JX3Dir\" (use '/' or '\\\\' as path separator)." << std::endl;
#ifdef _WIN32
        system("pause");
#endif
        return 0;
    }

    ns_program::Web webHandler;
    std::string     s;
    while (std::getline(std::cin, s)) {
        if (s == "exit")
            break;
    }
    webHandler.stop();

    return 0;
}
