#ifndef UTILS_CONFIG_H_
#define UTILS_CONFIG_H_

#include <filesystem>

namespace ns_utils {
namespace config {

inline std::filesystem::path pExeDir; // 可执行文件所在目录的路径
inline std::filesystem::path pUnpack; // 未打包的数据目录
inline std::filesystem::path pJX3;    // 已打包的数据目录

// 初始化环境.
void init(int argc, char *argv[]);

// 加载配置文件
void load();

}; // namespace config
} // namespace ns_utils

#endif // UTILS_CONFIG_H_
