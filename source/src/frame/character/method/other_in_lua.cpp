#include "frame/character/character.h"
#include "frame/event.h"
#include "frame/global/cooldown.h"
#include "frame/lua_runtime.h"
#include "program/log.h"

using namespace ns_frame;

bool Character::IsHaveBuff(int buffID, int buffLevel) {
    if (this->chBuff.buffList.find(std::make_tuple(buffID, buffLevel)) == this->chBuff.buffList.end()) {
        return false;
    } else {
        return this->chBuff.buffList[std::make_tuple(buffID, buffLevel)].isValid;
    }
}

static void staticCooldownOver(void *self, void *ID);
void Character::ModifyCoolDown(int ID, int frame) {
    if (0 == frame) {
        return;
    }
    ns_frame::CharacterCooldown::Item *item = &this->chCooldown.cooldownList[ID]; // Character. 若不存在, 则会自动创建
    ns_frame::Cooldown &cooldown = CooldownManager::get(ID);                      // Global
    event_tick_t delay = 0;
    if (item->isValid) {
        // 若该 CD 正在冷却, 则取消 Event
        delay = EventManager::cancel(item->tickOver, staticCooldownOver, this, &cooldown.ID);
        if (frame < 0 && -frame * 1024 / 16 > delay) {
            // 传入了一个负值的 frame, 且该值大于当前的冷却时间, 则直接结束冷却
            // 之所以将其放置在 item->isValid 中, 是因为只有当 CD 正在冷却时, 传入一个负值的 frame 才有意义
            staticCooldownOver(this, &cooldown.ID);
            return; // 直接返回
        }
    }
    // 若此时仍未 return, 说明仍需要进行冷却
    item->isValid = true;
    item->tickOver = EventManager::add(delay + frame * 1024 / 16, staticCooldownOver, this, &cooldown.ID);
}
static void staticCooldownOver(void *selfPtr, void *IDPtr) {
    Character *self = (Character *)selfPtr;
    int ID = *(int *)IDPtr;
    self->chCooldown.cooldownList[ID].isValid = false;
}

CharacterBuff::Item *Character::GetBuff(int buffID, int buffLevel) {
    if (this->chBuff.buffList.find(std::make_tuple(buffID, buffLevel)) == this->chBuff.buffList.end()) {
        return nullptr;
    } else {
        return &this->chBuff.buffList[std::make_tuple(buffID, buffLevel)];
    }
}

int Character::GetSkillLevel(int skillID) {
    if (this->chSkill.skillLearned.find(skillID) == this->chSkill.skillLearned.end()) {
        return 0;
    } else {
        return this->chSkill.skillLearned[skillID];
    }
}

static void staticOnTimer(void *self, void *param);
void Character::SetTimer(int frame, std::string filename, int type, int targetID) {
    int *data = new int[3];
    data[0] = LuaFunc::getIndex(filename);
    data[1] = type;
    data[2] = targetID;
    EventManager::add(frame * 1024 / 16, staticOnTimer, this, data);
}
static void staticOnTimer(void *self, void *param) {
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