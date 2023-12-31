#ifndef GLOBAL_SETTINGS_H_
#define GLOBAL_SETTINGS_H_

#include <filesystem>

namespace fs = std::filesystem;

namespace ns_global {

// 初始化环境.
void init(int argc, char *argv[]);

// 配置设置. 注意, 这是一个静态类, 无法创建实例.
class Config {
public:
    // 注意: 以下为静态成员变量, 必须在 .cpp 文件中初始化.
    static inline fs::path pExeDir; // 可执行文件所在目录的路径
    static inline fs::path pUnpack; // 未打包的数据目录
    static inline fs::path pJX3;    // 已打包的数据目录

    // 禁止创建类实例
    Config() = delete;
    // 加载配置文件
    static void load();
};

}; // namespace ns_global

#endif // GLOBAL_SETTINGS_H_
