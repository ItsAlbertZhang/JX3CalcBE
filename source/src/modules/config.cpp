#include "modules/config.h"
#include "gdi.h"
#include <format>
#include <fstream>
#include <nlohmann/json.hpp>

#define UNREFERENCED_PARAMETER(P) (P)

using namespace jx3calc;
using namespace modules;
using json   = nlohmann::json;
namespace fs = std::filesystem;

const std::string version = "v1.2.1.083101";

static bool initFromJson(const json &j) {
    using namespace config;
    std::string spSeasunGame;
    std::string spUnpack;
    if (j.contains("dirSeasunGame") && j.at("dirSeasunGame").is_string()) {
        spSeasunGame = j.at("dirSeasunGame").get<std::string>();
    }
    if (j.contains("dirUnpacked") && j.at("dirUnpacked").is_string()) {
        spUnpack = j.at("dirUnpacked").get<std::string>();
    }
    ClientType client = ClientType::jx3_hd;
    if (j.contains("clientType") && j.at("clientType").is_string()) {
        std::string s = j.at("clientType").get<std::string>();
        for (const auto &[key, value] : ClientTypeMap) {
            if (value.name == s) {
                client = key;
                break;
            }
        }
    }
    if (j.contains("isUTF8") && j.at("isUTF8").is_boolean()) {
        config::isUTF8 = j.at("isUTF8").get<bool>();
    }
    std::string spBin64 = std::format(
        "{}/Game/{}/bin/{}/bin64",
        spSeasunGame, ClientTypeMap.at(client).path1, ClientTypeMap.at(client).path2
    );
    if (0 == gdi::dataInit(spBin64.c_str(), spUnpack.c_str())) {
        clientType = client;
    } else {
        return false;
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
    if (j.contains("maxTaskDuration") && j["maxTaskDuration"].is_number_integer()) {
        config::taskdata::maxTaskDuration = j["maxTaskDuration"].get<int>();
    }
    return true;
}

bool config::init(int argc, char *argv[]) {
    UNREFERENCED_PARAMETER(argc);
    pExeDir = fs::absolute(argv[0]).parent_path();
    json     j;
    fs::path pathConfig = pExeDir / "config.json";
    if (fs::exists(pathConfig)) {
        std::ifstream fileConfig(pathConfig);
        fileConfig >> j;
    }
    return initFromJson(j);
}

bool config::configure(const std::string &jsonstr) {
    try {
        json     jNew       = json::parse(jsonstr);
        fs::path pathConfig = pExeDir / "config.json";
        if (jNew.contains("update") && fs::exists(pathConfig)) {
            json          jOld;
            std::ifstream fileConfig {pathConfig};
            fileConfig >> jOld;
            jOld.update(jNew);
            jNew = std::move(jOld);
            jNew.erase("update");
        }
        std::ofstream fileConfig {pathConfig};
        fileConfig << jNew.dump(4);
        return true;
    } catch (...) {
        return false;
    }
}

std::string config::status() {
    json j;
    j["status"] = clientType != ClientType::unknown ? 0 : -1;

    if (clientType != ClientType::unknown) {
        auto &data      = j["data"];
        data["version"] = version;

        auto &userinput               = data["userinput"];
        userinput["maxDelayNetwork"]  = taskdata::maxDelayNetwork;
        userinput["maxDelayKeyboard"] = taskdata::maxDelayKeyboard;
        userinput["maxFightTime"]     = taskdata::maxFightTime;
        userinput["maxFightCount"]    = taskdata::maxFightCount;

        data["custom"] = taskdata::allowCustom;
        data["client"] = ClientTypeMap.at(clientType).name;
    }
    return j.dump();
}
