#include "thread/main.h"
#include "frame/lua_runtime.h"

using namespace ns_thread;

void Main::cleanup() {
    ns_frame::LuaFunc::clear();
}

Main::Main()
    : child{}, pool{[]() {}, cleanup} {}
