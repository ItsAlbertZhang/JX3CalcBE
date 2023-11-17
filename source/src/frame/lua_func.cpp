#include "frame/lua_func.h"
#include "gdi.h"

using namespace ns_frame;
using namespace std;

thread_local unordered_map<string, sol::protected_function> LuaApply::data;

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
        std::cerr << "luaExecuteFile failed:" << name << "\n"
                  << err.what() << std::endl;
    } else {
        sol::protected_function func = gdi::Interface::luaGetFunction("Apply");
        data[name] = func;
    }
}