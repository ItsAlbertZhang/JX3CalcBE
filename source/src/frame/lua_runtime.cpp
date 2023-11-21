#include "frame/lua_runtime.h"
#include "gdi.h"
#include "program/log.h"

using namespace ns_frame;
using namespace std;

sol::protected_function LuaApply::get(string name) {
    if (data.find(name) == data.end()) {
        add(name);
    }
    return data[name];
}

void LuaApply::add(string name) {
    // 由于 data 是线程本地的, 因此不用考虑线程安全问题
    sol::protected_function_result res = gdi::Interface::luaExecuteFile(name);
    if (!res.valid()) {
        sol::error err = res;
        LOG_ERROR("luaExecuteFile failed: %s\n%s\n", name.c_str(), err.what());
    } else {
        sol::protected_function func = gdi::Interface::luaGetFunction("Apply");
        data[name] = func;
    }
}