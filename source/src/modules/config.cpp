#include "modules/config.h"
#include "gdi.h"
#include <fstream>
#include <nlohmann/json.hpp>

#define UNREFERENCED_PARAMETER(P) (P)

using namespace jx3calc;
using namespace modules;
using json   = nlohmann::json;
namespace fs = std::filesystem;

const std::string version = "v1.1.1.050101";

static inline bool stringContains(const std::string &str, const std::string &substr) {
    return str.find(substr) != std::string::npos;
}

static void initFromJson(const json &j) {
    using namespace config;
    std::string spJX3;
    std::string spUnpack;
    if (j.contains("JX3Dir") && j["JX3Dir"].is_string()) {
        spJX3 = j["JX3Dir"].get<std::string>();
    }
    if (j.contains("UnpackDir") && j["UnpackDir"].is_string()) {
        spUnpack = j["UnpackDir"].get<std::string>();
    }
    if (0 == gdi::dataInit(spJX3.c_str(), spUnpack.c_str())) {
        if (stringContains(spJX3, "exp") || stringContains(spUnpack, "exp") ||
            stringContains(spJX3, "EXP") || stringContains(spUnpack, "EXP"))
            dataAvailable = dataStatus::jx3_exp;
        else
            dataAvailable = dataStatus::jx3;
    }

    if (j.contains("maxDelayNetwork") && j["maxDelayNetwork"].is_number_integer()) {
        config::taskdata::maxDelayNetwork = j["maxDelayNetwork"].get<int>();
    }
    if (j.contains("maxDelayKeyboard") && j["maxDelayKeyboard"].is_number_integer()) {
        config::taskdata::maxDelayKeyboard = j["maxDelayKeyboard"].get<int>();
    }
    if (j.contains("maxFightTime") && j["maxFightTime"].is_number_integer()) {
        config::taskdata::maxFightTime = j["maxFightTime"].get<int>();
    }
    if (j.contains("maxFightCount") && j["maxFightCount"].is_number_integer()) {
        config::taskdata::maxFightCount = j["maxFightCount"].get<int>();
    }
    if (j.contains("allowCustom") && j["allowCustom"].is_boolean()) {
        config::taskdata::allowCustom = j["allowCustom"].get<bool>();
    }
}

void config::init(int argc, char *argv[]) {
    UNREFERENCED_PARAMETER(argc);
    pExeDir = fs::absolute(argv[0]).parent_path();
    json     j;
    fs::path pathConfig = pExeDir / "config.json";
    if (fs::exists(pathConfig)) {
        std::ifstream fileConfig(pathConfig);
        fileConfig >> j;
    }
    initFromJson(j);
}

bool config::init(const std::string &jsonstr) {
    try {
        json     j;
        fs::path pathConfig = pExeDir / "config.json";
        if (fs::exists(pathConfig)) {
            std::ifstream fileConfig{pathConfig};
            fileConfig >> j;
        }
        j.update(json::parse(jsonstr));
        initFromJson(j);
        if (dataAvailable == dataStatus::unavailable) {
            return false;
        }
        std::ofstream fileConfig{pathConfig};
        fileConfig << j.dump(4);
        return true;
    } catch (...) {
        return false;
    }
}

std::string config::status() {
    json j;
    j["status"] = dataAvailable != dataStatus::unavailable ? 0 : -1;

    if (dataAvailable != dataStatus::unavailable) {
        auto &data      = j["data"];
        data["version"] = version;

        auto &userinput               = data["userinput"];
        userinput["maxDelayNetwork"]  = taskdata::maxDelayNetwork;
        userinput["maxDelayKeyboard"] = taskdata::maxDelayKeyboard;
        userinput["maxFightTime"]     = taskdata::maxFightTime;
        userinput["maxFightCount"]    = taskdata::maxFightCount;

        data["custom"] = taskdata::allowCustom;
        data["isExp"]  = dataAvailable == dataStatus::jx3_exp;
    }
    return j.dump();
}
