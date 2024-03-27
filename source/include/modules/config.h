#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>

namespace jx3calc {
namespace modules {
namespace config {

inline std::filesystem::path pExeDir;               // 可执行文件所在目录的路径
inline bool                  dataAvailable = false; // 数据是否已经初始化

namespace taskdata {

inline int  maxDelayNetwork  = 1024;    // 最大网络延迟
inline int  maxDelayKeyboard = 1024;    // 最大按键延迟
inline int  maxFightTime     = 1 << 30; // 最大战斗时间
inline int  maxFightCount    = 1 << 30; // 最大战斗次数
inline bool allowCustom      = true;    // 是否允许自定义战斗

} // namespace taskdata

// 从 main 函数中初始化数据
void init(int argc, char *argv[]);
// 从字符串中初始化数据, 并保存为配置文件
bool init(const std::string &jsonstr);

// 返回当前配置的状态
std::string status();

}; // namespace config
} // namespace modules
} // namespace jx3calc
