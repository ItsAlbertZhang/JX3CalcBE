#include "utils/config.h"
#include "gdi.h"
#include <fstream>
#include <nlohmann/json.hpp>

#define UNREFERENCED_PARAMETER(P) (P)
namespace fs = std::filesystem;

// 初始化 exe 目录
void ns_utils::config::initExeDir(int argc, char *argv[]) {
    UNREFERENCED_PARAMETER(argc);
    pExeDir = fs::absolute(argv[0]).parent_path();
}

// 从配置文件中初始化数据
void ns_utils::config::initDataFromLocalFile() {
    fs::path pathConfig = pExeDir / "config.json";
    if (!fs::exists(pathConfig)) {
        return;
    }
    std::ifstream  fileConfig(pathConfig);
    nlohmann::json j;
    fileConfig >> j;
    std::string spJX3;
    std::string spUnpack;
    if (j.contains("JX3Dir")) {
        spJX3 = j["JX3Dir"].get<std::string>();
    }
    if (j.contains("UnpackDir")) {
        spUnpack = j["UnpackDir"].get<std::string>();
    }
    gdi::initData(spJX3, spUnpack);
    dataAvailable = true;
}

// 从字符串中初始化数据, 并保存为配置文件
void ns_utils::config::initDataFromString(const std::string &jsonstr) {
    nlohmann::json j = nlohmann::json::parse(jsonstr);
    std::string    spJX3;
    std::string    spUnpack;
    if (j.contains("JX3Dir")) {
        spJX3 = j["JX3Dir"].get<std::string>();
    }
    if (j.contains("UnpackDir")) {
        spUnpack = j["UnpackDir"].get<std::string>();
    }
    gdi::initData(spJX3, spUnpack);
    dataAvailable            = true;
    // 将其保存为配置文件并覆盖
    fs::path      pathConfig = pExeDir / "config.json";
    std::ofstream fileConfig(pathConfig);
    fileConfig << j.dump(4);
}
