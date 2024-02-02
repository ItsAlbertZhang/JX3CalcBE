#ifndef UTILS_CONFIG_H_
#define UTILS_CONFIG_H_

#include <filesystem>
#include <nlohmann/json.hpp>

namespace ns_utils {
namespace config {

inline std::filesystem::path pExeDir; // 可执行文件所在目录的路径
inline std::filesystem::path pUnpack; // 未打包的数据目录
inline std::filesystem::path pJX3;    // 已打包的数据目录

namespace taskdata {

inline int  maxDelayNetwork  = 1024;    // 最大网络延迟
inline int  maxDelayKeyboard = 1024;    // 最大按键延迟
inline int  maxFightTime     = 1 << 30; // 最大战斗时间
inline int  maxFightCount    = 1 << 30; // 最大战斗次数
inline bool allowCustomMacro = true;    // 是否允许自定义宏

std::string genSchema();
} // namespace taskdata

// 初始化环境.
void init(int argc, char *argv[]);

// 加载配置文件
void load();

}; // namespace config
} // namespace ns_utils

#endif // UTILS_CONFIG_H_
