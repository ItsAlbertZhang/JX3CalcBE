#include "frame/lua_runtime.h"
#include "gdi.h"
#include "program/log.h"

using namespace ns_frame;
using namespace std;

sol::protected_function LuaFunc::getApply(string filename) {
    if (data.find(filename) == data.end()) {
        add(filename);
    }
    if (data.find(filename) != data.end() && data[filename].size() == static_cast<int>(Enum::COUNT)) {
        return data[filename][static_cast<int>(Enum::Apply)];
    } else {
        return sol::nil;
    }
}

void LuaFunc::add(std::string filename) {
    // 由于 data 是线程本地的, 因此不用考虑线程安全问题
    sol::protected_function_result res = gdi::Interface::luaExecuteFile(filename);
    if (!res.valid()) {
        sol::error err = res;
        LOG_ERROR("luaExecuteFile failed: %s\n%s\n", filename.c_str(), err.what());
    } else {
        std::vector<sol::protected_function> &funcs = data[filename];
        for (int i = 0; i < static_cast<int>(Enum::COUNT); i++) {
            funcs.emplace_back(gdi::Interface::luaGetFunction(names[i]));
        }
    }
}