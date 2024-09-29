#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>

namespace jx3calc {
namespace modules {
namespace config {

enum class ClientType {
    unknown = 0,
    jx3_hd,
    jx3_exp,
};
struct ClientStruct {
    std::string name;
    std::string path1;
    std::string path2;
};
inline std::unordered_map<ClientType, ClientStruct> ClientTypeMap {
    {ClientType::jx3_hd, {"jx3_hd", "JX3", "zhcn_hd"}},
    {ClientType::jx3_exp, {"jx3_exp", "JX3_EXP", "zhcn_exp"}},
};

inline std::filesystem::path pExeDir;                          // 可执行文件所在目录的路径
inline ClientType            clientType = ClientType::unknown; // 客户端类型
inline bool                  isUTF8     = false;               // 本地文件是否为 UTF-8 编码

namespace taskdata {

inline int  maxDelayNetwork  = 1024;    // 最大网络延迟
inline int  maxDelayKeyboard = 1024;    // 最大按键延迟
inline int  maxFightTime     = 1 << 30; // 最大战斗时间
inline int  maxFightCount    = 1 << 30; // 最大战斗次数
inline bool allowCustom      = true;    // 是否允许自定义战斗
inline int  maxTaskDuration  = 1 << 30; // 最大任务持续时间

} // namespace taskdata

// 从 main 函数中初始化数据
bool init(int argc, char *argv[]);
// 从字符串中初始化数据, 并保存为配置文件
bool configure(const std::string &jsonstr);

// 返回当前配置的状态
std::string status();

// 判断是否为测试服. 正式服更新合并测试服时, 注意检查此函数调用.
// 注意: 仅限全局相关内容使用这个函数. 如果是 concrete 相关, 应使用模板实例化.
inline bool isExp() {
    return clientType == ClientType::jx3_exp;
}

}; // namespace config
} // namespace modules
} // namespace jx3calc
