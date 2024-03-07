#include "frame/global/cooldown.h"
#include "frame/character/character.h"
#include "frame/character/property/buff.h"
#include "frame/event.h"

using namespace ns_frame;

static event_tick_t calcDuration(Character *self, const ns_frame::Cooldown &cooldown) {
    int durationFrame = cooldown.DurationFrame * 1024 / (1024 + self->chAttr.getHaste());
    durationFrame     = durationFrame > cooldown.MinDurationFrame ? durationFrame : cooldown.MinDurationFrame;
    durationFrame     = durationFrame < cooldown.MaxDurationFrame ? durationFrame : cooldown.MaxDurationFrame;
    return durationFrame * 1024 / 16;
}

static void callbackModifyCoolDown(void *selfPtr, void *cooldownID) {
    // 利用 void * 传递一个 int 值, 避免内存的申请与回收.
    Character                  *self     = (Character *)selfPtr;
    int                         ID       = static_cast<int>(reinterpret_cast<intptr_t>(cooldownID));
    // self->chCooldown.cooldownList[ID].isValid = false;
    const ns_frame::Cooldown   &cooldown = CooldownManager::get(ID); // Global
    ns_frame::ChCooldown::Item &item     = self->chCooldown.cooldownList[ID];
    item.countAvailable += 1;
    if (item.countAvailable < cooldown.MaxCount) {
        // 重新计算 duration, 实现受加速影响的充能技能的动态 duration
        event_tick_t duration = calcDuration(self, cooldown);
        // 如果 tickAdditional 更短, 则使用 tickAdditional.
        // 如在高加速下使用了前两段, 随后加速恢复正常, 使用第三段. 此时倒数第二层的冷却是正常值, 但最后一层的冷却会极短.
        duration              = duration > item.tickAdditional ? item.tickAdditional : duration;
        item.tickOver         = Event::add(duration, callbackModifyCoolDown, self, cooldownID);
        item.tickAdditional -= duration;
    }
}

static ns_frame::ChCooldown::Item *getOrCreate(Character *self, const ns_frame::Cooldown &cooldown) {
    if (self->chCooldown.cooldownList.contains(cooldown.ID)) {
        return &self->chCooldown.cooldownList[cooldown.ID];
    } else {
        ns_frame::ChCooldown::Item *res = &self->chCooldown.cooldownList[cooldown.ID];
        res->countAvailable             = cooldown.MaxCount;
        return res;
    }
}

void Character::cooldownClearTime(int cooldownID) {
    if (this->chCooldown.cooldownList.find(cooldownID) == this->chCooldown.cooldownList.end()) {
        return;
    }
    ChCooldown::Item         &item     = this->chCooldown.cooldownList.at(cooldownID);
    const ns_frame::Cooldown &cooldown = CooldownManager::get(cooldownID); // Global
    if (item.countAvailable < cooldown.MaxCount) {
        Event::cancel(item.tickOver, callbackModifyCoolDown, this, reinterpret_cast<void *>(static_cast<intptr_t>(cooldown.ID)));
        item.countAvailable = cooldown.MaxCount;
        item.tickAdditional = 0;
    }
}

void Character::cooldownModify(int cooldownID, int frame) {
    if (0 == frame) {
        return;
    }
    const ns_frame::Cooldown   &cooldown = CooldownManager::get(cooldownID); // Global
    ns_frame::ChCooldown::Item *item     = getOrCreate(this, cooldown);
    event_tick_t                delay    = 0;
    if (item->countAvailable < cooldown.MaxCount) {
        // 若该 CD 正在冷却, 则取消 Event
        delay = Event::cancel(item->tickOver, callbackModifyCoolDown, this, reinterpret_cast<void *>(static_cast<intptr_t>(cooldown.ID)));
    } else {
        // 特判. CD 不在冷却中可以变相认为是: 离满层还差 1 层, 并且还有 0 tick 的冷却时间.
        item->countAvailable--;
    }
    event_tick_t duration        = calcDuration(this, cooldown);
    event_tick_t delayToMaxCount = delay + item->tickAdditional + frame * 1024 / 16;
    // 当前层的剩余冷却时间 + 其他层数的总冷却时间 + 本次修改的冷却时间
    if (delayToMaxCount <= 0) {
        item->countAvailable = cooldown.MaxCount;
        item->tickAdditional = 0;
        return;
    }
    int cnt = static_cast<int>(delayToMaxCount / duration) + 1; // 与满层的层数差. 至少是 1
    delay   = delayToMaxCount % duration;                       // 当前层的剩余冷却时间
    if (cnt > cooldown.MaxCount) {
        // 如果与满层的层数差已经超出了最大可用次数
        delay += (cnt - cooldown.MaxCount) * duration; // 超出部分退还为当前层的剩余冷却时间
        cnt = cooldown.MaxCount;                       // 修正为最大可用次数
    }
    item->countAvailable = cooldown.MaxCount - cnt;
    item->tickAdditional = delayToMaxCount - delay;
    item->tickOver       = Event::add(delay, callbackModifyCoolDown, this, reinterpret_cast<void *>(static_cast<intptr_t>(cooldown.ID)));
}

void Character::cooldownReset(int cooldownID) {
    const ns_frame::Cooldown   &cooldown = CooldownManager::get(cooldownID); // Global
    ns_frame::ChCooldown::Item *item     = getOrCreate(this, cooldown);
    if (item->countAvailable < cooldown.MaxCount) {
        // 若该 CD 正在冷却, 则取消 Event
        Event::cancel(item->tickOver, callbackModifyCoolDown, this, reinterpret_cast<void *>(static_cast<intptr_t>(cooldown.ID)));
    }
    item->countAvailable  = 0;
    event_tick_t duration = calcDuration(this, cooldown);
    item->tickAdditional  = duration * (cooldown.MaxCount - 1);
    item->tickOver        = Event::add(duration, callbackModifyCoolDown, this, reinterpret_cast<void *>(static_cast<intptr_t>(cooldown.ID)));
}
