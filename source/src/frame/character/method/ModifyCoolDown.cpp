#include "frame/character/character.h"
#include "frame/event.h"
#include "frame/global/cooldown.h"

using namespace ns_frame;

static void callbackModifyCoolDown(void *selfPtr, void *cooldownIDPtr) {
    Character *self = (Character *)selfPtr;
    int ID = *(int *)cooldownIDPtr;
    self->chCooldown.cooldownList[ID].isValid = false;
}
void Character::ModifyCoolDown(int cooldownID, int frame) {
    if (0 == frame) {
        return;
    }
    ns_frame::CharacterCooldown::Item *item = &this->chCooldown.cooldownList[cooldownID]; // Character. 若不存在, 则会自动创建
    ns_frame::Cooldown &cooldown = CooldownManager::get(cooldownID);                      // Global
    event_tick_t delay = 0;
    if (item->isValid) {
        // 若该 CD 正在冷却, 则取消 Event
        delay = EventManager::cancel(item->tickOver, callbackModifyCoolDown, this, &cooldown.ID);
        if (frame < 0 && -frame * 1024 / 16 > delay) {
            // 传入了一个负值的 frame, 且该值大于当前的冷却时间, 则直接结束冷却
            // 之所以将其放置在 item->isValid 中, 是因为只有当 CD 正在冷却时, 传入一个负值的 frame 才有意义
            callbackModifyCoolDown(this, &cooldown.ID);
            return; // 直接返回
        }
    }
    // 若此时仍未 return, 说明仍需要进行冷却
    item->isValid = true;
    item->tickOver = EventManager::add(delay + frame * 1024 / 16, callbackModifyCoolDown, this, &cooldown.ID);
}