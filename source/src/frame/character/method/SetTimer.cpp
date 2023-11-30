#include "frame/character/character.h"
#include "frame/event.h"
#include "frame/runtime_lua.h"
#include "program/log.h"

using namespace ns_frame;

static void callbackSetTimer(void *self, void *param) {
    Character *selfPtr = (Character *)self;
    int *data = (int *)param;
    int idx = data[0];
    int type = data[1];
    int targetID = data[2];
    delete[] data;
    LuaFunc::analysis(LuaFunc::getOnTimer(idx)(selfPtr, type, targetID), idx, LuaFunc::Enum::OnTimer);
}

void Character::SetTimer3(int frame, std::string filename, int targetID) {
    this->SetTimer4(frame, filename, getCharacter(targetID)->isPlayer ? 1 : 0, targetID);
}

void Character::SetTimer4(int frame, std::string filename, int type, int targetID) {
    int *data = new int[3];
    data[0] = LuaFunc::getIndex(filename);
    data[1] = type;
    data[2] = targetID;
    Event::add(frame * 1024 / 16, callbackSetTimer, this, data);
}
