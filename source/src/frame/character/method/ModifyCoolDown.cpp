#include "frame/character/character.h"
#include "frame/event.h"
#include "frame/global/cooldown.h"
#include <cstdint>

using namespace ns_frame;

static void callbackModifyCoolDown(void *selfPtr, void *cooldownID) {
    // 利用 void * 传递一个 int 值, 避免内存的申请与回收.
    Character *self = (Character *)selfPtr;
    int ID = static_cast<int>(reinterpret_cast<intptr_t>(cooldownID));
    self->chCooldown.cooldownList[ID].isValid = false;
}
void Character::ModifyCoolDown(int cooldownID, int frame) {
    if (0 == frame) {
        return;
    }
    ns_frame::CharacterCooldown::Item *item = &this->chCooldown.cooldownList[cooldownID]; // Character. 若不存在, 则会自动创建
    const ns_frame::Cooldown &cooldown = CooldownManager::get(cooldownID);                // Global
    event_tick_t delay = 0;
    if (item->isValid) {
        // 若该 CD 正在冷却, 则取消 Event
        delay = Event::cancel(item->tickOver, callbackModifyCoolDown, this, reinterpret_cast<void *>(static_cast<intptr_t>(cooldown.ID)));
    }
    if (frame < 0 && -frame * 1024 / 16 >= delay) {
        // 传入了一个负值的 frame, 且该值大于等于当前的冷却时间, 则直接结束冷却
        item->isValid = false;
    } else {
        item->isValid = true;
        item->tickOver = Event::add(delay + frame * 1024 / 16, callbackModifyCoolDown, this, reinterpret_cast<void *>(static_cast<intptr_t>(cooldown.ID)));
    }
}