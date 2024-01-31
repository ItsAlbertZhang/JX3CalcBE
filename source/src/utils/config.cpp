#include "utils/config.h"
#include "concrete/character/character.h"
#include "concrete/effect/effect.h"
#include "modules/task.h"
#include <fstream>
#include <nlohmann/json.hpp>

#define UNREFERENCED_PARAMETER(P) (P)

void ns_utils::config::init(int argc, char *argv[]) { // 初始化所有环境
    namespace fs = std::filesystem;
    UNREFERENCED_PARAMETER(argc);
    pExeDir = fs::absolute(argv[0]).parent_path();
    load();
}

void ns_utils::config::load() { // 加载配置文件
    namespace fs        = std::filesystem;
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

void ns_utils::config::schemaTaskDataComplete(nlohmann::json &j) {
    for (auto &x : ns_concrete::PlayerTypeMap) {
        j["properties"]["player"]["enum"].push_back(x.first);
    }
    for (auto &x : ns_modules::task::AttributeTypeMap) {
        j["properties"]["attribute"]["enum"].push_back(x.first);
    }
    for (auto &x : ns_concrete::EffectTypeMap) {
        j["properties"]["effects"]["items"]["enum"].push_back(x.first);
    }
}

nlohmann::json ns_utils::config::schemaTaskDataInit() {
    nlohmann::json ret{
        {"type",       "object"                },
        {"properties", nlohmann::json::object()},
        {"required",   nlohmann::json::array() },
    };

    ret["properties"]["player"] = {
        {"type", "string"               },
        {"enum", nlohmann::json::array()},
    };
    ret["required"].push_back("player");

    ret["properties"]["delayNetwork"] = {
        {"type",    "integer"},
        {"minimum", 0        },
        {"maximum", 1024     },
    };
    ret["required"].push_back("delayNetwork");

    ret["properties"]["delayKeyboard"] = {
        {"type",    "integer"},
        {"minimum", 0        },
        {"maximum", 1024     },
    };
    ret["required"].push_back("delayKeyboard");

    ret["properties"]["fightTime"] = {
        {"type",    "integer"},
        {"minimum", 0        },
        {"maximum", 1 << 30  },
    };
    ret["required"].push_back("fightTime");

    ret["properties"]["fightCount"] = {
        {"type",    "integer"},
        {"minimum", 0        },
        {"maximum", 1 << 30  },
    };
    ret["required"].push_back("fightCount");

    ret["properties"]["customMacro"] = {
        {"type", "string"},
    };

    ret["properties"]["attribute"] = ns_modules::task::schemaAttribute();
    ret["required"].push_back("attribute");

    ret["properties"]["effects"] = {
        {"type", "array"},
    };
    ret["properties"]["effects"]["items"] = {
        {"type", "string"               },
        {"enum", nlohmann::json::array()},
    };
    ret["required"].push_back("effects");

    schemaTaskDataComplete(ret);

    return ret;
}
