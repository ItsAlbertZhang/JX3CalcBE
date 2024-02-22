#ifndef UTILS_CONFIG_H_
#define UTILS_CONFIG_H_

#include <filesystem>
#include <nlohmann/json.hpp>

namespace ns_utils {
namespace config {

inline std::filesystem::path pExeDir;               // 可执行文件所在目录的路径
inline bool                  dataAvailable = false; // 数据是否已经初始化

namespace taskdata {

inline int  maxDelayNetwork  = 1024;    // 最大网络延迟
inline int  maxDelayKeyboard = 1024;    // 最大按键延迟
inline int  maxFightTime     = 1 << 30; // 最大战斗时间
inline int  maxFightCount    = 1 << 30; // 最大战斗次数
inline bool allowCustomMacro = true;    // 是否允许自定义宏

} // namespace taskdata

// 初始化 exe 目录
void initExeDir(int argc, char *argv[]);

// 从配置文件中初始化数据
void initDataFromLocalFile();

// 从字符串中初始化数据, 并保存为配置文件
void initDataFromString(const std::string &jsonstr);

}; // namespace config
} // namespace ns_utils

#endif // UTILS_CONFIG_H_
