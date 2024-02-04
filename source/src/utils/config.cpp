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

std::string ns_utils::config::taskdata::genSchema() {
    using json = nlohmann::json;
    json ret{
        {"type",       "object"      },
        {"required",   json::array() },
        {"sort",       json::array() },
        {"properties", json::object()},
    };

    ret["properties"]["player"] = {
        {"type",    "integer"                        },
        {"title",   "心法"                         },
        {"minimum", 0                                },
        {"maximum", ns_concrete::refPlayerType.size()},
        {"list",    json::array()                    },
    };
    for (auto &x : ns_concrete::refPlayerType) {
        ret["properties"]["player"]["list"].push_back(x);
    }
    ret["required"].push_back("player");
    ret["sort"].push_back("player");

    ret["properties"]["delay"] = {
        {"type",       "object"               },
        {"required",   {"network", "keyboard"}},
        {"sort",       {"network", "keyboard"}},
        {"properties", json::object()         },
    };
    ret["properties"]["delay"]["properties"]["network"] = {
        {"type",    "integer"      },
        {"title",   "网络延迟" },
        {"minimum", 0              },
        {"maximum", maxDelayNetwork},
    };
    ret["properties"]["delay"]["properties"]["keyboard"] = {
        {"type",    "integer"       },
        {"title",   "按键间隔"  },
        {"minimum", 0               },
        {"maximum", maxDelayKeyboard},
    };
    ret["required"].push_back("delay");
    ret["sort"].push_back("delay");

    ret["properties"]["fight"] = {
        {"type",       "object"         },
        {"required",   {"time", "count"}},
        {"sort",       {"time", "count"}},
        {"properties", json::object()   },
    };
    ret["properties"]["fight"]["properties"]["time"] = {
        {"type",    "integer"     },
        {"title",   "战斗时间"},
        {"minimum", 0             },
        {"maximum", maxFightTime  },
    };
    ret["properties"]["fight"]["properties"]["count"] = {
        {"type",    "integer"     },
        {"title",   "战斗次数"},
        {"minimum", 0             },
        {"maximum", maxFightCount },
    };
    ret["required"].push_back("fight");
    ret["sort"].push_back("fight");

    ret["properties"]["attribute"] = ns_modules::task::schemaAttribute();
    ret["required"].push_back("attribute");
    ret["sort"].push_back("attribute");

    ret["properties"]["effects"] = {
        {"type",  "array"       },
        {"title", "增益选择"},
        {"items", json::object()},
    };
    ret["properties"]["effects"]["items"] = {
        {"type",    "integer"                        },
        {"minimum", 0                                },
        {"maximum", ns_concrete::refEffectType.size()},
        {"list",    json::array()                    },
    };
    for (auto &x : ns_concrete::refEffectType) {
        ret["properties"]["effects"]["items"]["list"].push_back(x);
    }
    ret["required"].push_back("effects");
    ret["sort"].push_back("effects");

    if (allowCustomMacro) {
        ret["properties"]["customMacro"] = {
            {"type",  "string"      },
            {"title", "自定义宏"}
        };
        ret["sort"].push_back("customMacro");
    }

    return ret.dump();
}
