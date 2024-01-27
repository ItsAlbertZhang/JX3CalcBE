#include "global/config.h"
#include <fstream>
#include <nlohmann/json.hpp>

#define UNREFERENCED_PARAMETER(P) (P)

void nsg_config::init(int argc, char *argv[]) { // 初始化所有环境
    UNREFERENCED_PARAMETER(argc);
    pExeDir = fs::absolute(argv[0]).parent_path();
    load();
}

void nsg_config::load() { // 加载配置文件
    fs::path pathConfig = pExeDir / "config.json";
    if (!fs::exists(pathConfig)) {
        return;
    }
    std::ifstream  fileConfig(pathConfig);
    nlohmann::json j;
    fileConfig >> j;
    if (j.contains("JX3Dir")) {
        pJX3 = j["JX3Dir"].get<std::string>();
    }
    if (j.contains("UnpackDir")) {
        pUnpack = j["UnpackDir"].get<std::string>();
    }
}
