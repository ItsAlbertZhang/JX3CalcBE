#include "frame/character/character.h"
#include "frame/event.h"
#include "frame/lua_runtime.h"
#include "program/log.h"

using namespace ns_frame;

static void callbackSetTimer(void *self, void *param) {
    Character *selfPtr = (Character *)self;
    int *data = (int *)param;
    int idx = data[0];
    int type = data[1];
    int targetID = data[2];
    delete[] data;
    sol::protected_function_result res = LuaFunc::getOnTimer(idx)(selfPtr, type, targetID);
    if (!res.valid()) {
        sol::error err = res;
        LOG_ERROR("LuaFunc::getOnTimer failed: %s\n", err.what());
    }
}
void Character::SetTimer(int frame, std::string filename, int type, int targetID) {
    int *data = new int[3];
    data[0] = LuaFunc::getIndex(filename);
    data[1] = type;
    data[2] = targetID;
    EventManager::add(frame * 1024 / 16, callbackSetTimer, this, data);
}
