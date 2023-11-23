#include "frame/lua_runtime.h"
#include "gdi.h"
#include "program/log.h"

using namespace ns_frame;
using namespace std;

int LuaFunc::getIndex(string &filename) {
    std::replace(filename.begin(), filename.end(), '\\', '/');
    if (filenameMap.find(filename) == filenameMap.end()) {
        add(filename);
    }
    return filenameMap[filename];
}

const string &LuaFunc::getFilename(int idx) {
    return filenameList[idx];
}

sol::protected_function LuaFunc::getGetSkillLevelData(string &filename) {
    std::replace(filename.begin(), filename.end(), '\\', '/');
    int idx = getIndex(filename); // 执行 getIndex 后, 一定存在
    return filefuncList[idx][static_cast<int>(Enum::GetSkillLevelData)];
}

sol::protected_function LuaFunc::getApply(string &filename) {
    std::replace(filename.begin(), filename.end(), '\\', '/');
    int idx = getIndex(filename); // 执行 getIndex 后, 一定存在
    return filefuncList[idx][static_cast<int>(Enum::Apply)];
}

sol::protected_function LuaFunc::getOnTimer(int idx) {
    // 传入的 idx 是先前通过 getIndex 获取的, 一定存在
    return filefuncList[idx][static_cast<int>(Enum::OnTimer)];
}

bool LuaFunc::analysis(sol::protected_function_result res, std::string &filename, Enum func) {
    std::replace(filename.begin(), filename.end(), '\\', '/');
    if (!res.valid()) {
        sol::error err = res;
        LOG_ERROR("%s %s() failed: \n%s\n", filename.c_str(), names[static_cast<int>(func)].c_str(), err.what());
        return false;
    } else {
        LOG_INFO("%s %s() success\n", filename.c_str(), names[static_cast<int>(func)].c_str());
        return true;
    }
}

bool LuaFunc::analysis(sol::protected_function_result res, int idx, Enum func) {
    // 传入的 idx 是先前通过 getIndex 获取的, 一定存在
    if (!res.valid()) {
        sol::error err = res;
        LOG_ERROR("%s %s() failed: \n%s\n", names[idx].c_str(), names[static_cast<int>(func)].c_str(), err.what());
        return false;
    } else {
        LOG_INFO("%s %s() success\n", names[idx].c_str(), names[static_cast<int>(func)].c_str());
        return true;
    }
}

void LuaFunc::add(const std::string &filename) {
    // 由于 data 是线程本地的, 因此不用考虑线程安全问题
    filenameMap[filename] = static_cast<int>(filefuncList.size());
    std::vector<sol::protected_function> &funcs = filefuncList.emplace_back();
    bool executeSuccess = false;
    sol::protected_function_result res = gdi::Interface::luaExecuteFile(filename);
    if (!res.valid()) {
        sol::error err = res;
        LOG_ERROR("luaExecuteFile failed: %s\n%s\n", filename.c_str(), err.what());
    } else {
        executeSuccess = true;
    }
    for (int i = 0; i < static_cast<int>(Enum::COUNT); i++) {
        funcs.emplace_back(executeSuccess ? gdi::Interface::luaGetFunction(names[i]) : sol::nil);
    }
}