#include "frame/lua_runtime.h"
#include "gdi.h"
#include "program/log.h"

using namespace ns_frame;
using namespace std;

int LuaFunc::getIndex(const string &filename) {
    if (filenameMap.find(filename) == filenameMap.end()) {
        add(filename);
    }
    return filenameMap[filename];
}

sol::protected_function LuaFunc::getApply(const string &filename) {
    int idx = getIndex(filename);
    return filefuncList[idx][static_cast<int>(Enum::Apply)]; // 无需检查, 一定存在
}

sol::protected_function LuaFunc::getOnTimer(int idx) {
    return filefuncList[idx][static_cast<int>(Enum::OnTimer)]; // 无需检查, 一定存在
}

void LuaFunc::add(const std::string &filename) {
    // 由于 data 是线程本地的, 因此不用考虑线程安全问题
    sol::protected_function_result res = gdi::Interface::luaExecuteFile(filename);
    if (!res.valid()) {
        sol::error err = res;
        LOG_ERROR("luaExecuteFile failed: %s\n%s\n", filename.c_str(), err.what());
    } else {
        filenameMap[filename] = static_cast<int>(filefuncList.size());
        std::vector<sol::protected_function> &funcs = filefuncList.emplace_back();
        for (int i = 0; i < static_cast<int>(Enum::COUNT); i++) {
            funcs.emplace_back(gdi::Interface::luaGetFunction(names[i]));
        }
    }
}