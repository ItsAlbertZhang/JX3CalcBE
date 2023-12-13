#include "program/init.h"
#include <fstream>
#include <nlohmann/json.hpp>

#define UNREFERENCED_PARAMETER(P) (P)

using namespace ns_program;

void Init::init(int argc, char *argv[]) { // 初始化所有环境
    UNREFERENCED_PARAMETER(argc);
    Config::pExeDir = fs::absolute(argv[0]).parent_path();
    Config::load();
}

void Config::load() { // 加载配置文件
    fs::path pathConfig = Config::pExeDir / "config.json";
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
