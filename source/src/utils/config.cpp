#include "utils/config.h"
#include "gdi.h"
#include <fstream>
#include <nlohmann/json.hpp>

#define UNREFERENCED_PARAMETER(P) (P)
namespace fs = std::filesystem;
using namespace ns_utils;
using json = nlohmann::json;

const std::string version = "v1.0.0";

static void initFromJson(const json &j) {
    std::string spJX3;
    std::string spUnpack;
    if (j.contains("JX3Dir") && j["JX3Dir"].is_string()) {
        spJX3 = j["JX3Dir"].get<std::string>();
    }
    if (j.contains("UnpackDir") && j["UnpackDir"].is_string()) {
        spUnpack = j["UnpackDir"].get<std::string>();
    }
    config::dataAvailable = gdi::initData(spJX3, spUnpack);

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
    if (j.contains("allowCustomMacro") && j["allowCustomMacro"].is_boolean()) {
        config::taskdata::allowCustomMacro = j["allowCustomMacro"].get<bool>();
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
        if (!dataAvailable) {
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
    j["status"] = dataAvailable ? 0 : -1;
    if (dataAvailable) {

        auto &data      = j["data"];
        data["version"] = version;

        auto &userinput               = data["userinput"];
        userinput["maxDelayNetwork"]  = taskdata::maxDelayNetwork;
        userinput["maxDelayKeyboard"] = taskdata::maxDelayKeyboard;
        userinput["maxFightTime"]     = taskdata::maxFightTime;
        userinput["maxFightCount"]    = taskdata::maxFightCount;
        userinput["allowCustomMacro"] = taskdata::allowCustomMacro;
    }
    return j.dump();
}
