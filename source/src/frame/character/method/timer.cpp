#include "frame/character/character.h"
#include "frame/event.h"
#include "frame/lua_runtime.h"
#include "global/constexpr_log.h"

using namespace ns_frame;

union Data {
    struct {
        uint16_t idx;
        uint16_t type;
        uint32_t targetID;
    } data;
    void *param; // 3202 年了, 希望不会有人还在用 32 位系统.
};

static void callbackSetTimer(void *self, void *param) {
    Character *selfPtr = (Character *)self;
    Data       data{.param = param};
    if (!LuaFunc::analysis(LuaFunc::getOnTimer(data.data.idx)(selfPtr, data.data.type, data.data.targetID), data.data.idx, LuaFunc::Enum::OnTimer))
        CONSTEXPR_LOG_ERROR("LuaFunc::getOnTimer() failed.{}", "");
}

void Character::timerSet3(int frame, std::string filename, int targetID) {
    this->timerSet4(frame, filename, characterGet(targetID)->isPlayer ? 1 : 0, targetID);
}

void Character::timerSet4(int frame, std::string filename, int type, int targetID) {
    Data data{
        .data = {static_cast<uint16_t>(LuaFunc::getIndex(filename)), static_cast<uint16_t>(type), static_cast<uint32_t>(targetID)}
    };
    Event::add(frame * 1024 / 16, callbackSetTimer, this, data.param);
}
