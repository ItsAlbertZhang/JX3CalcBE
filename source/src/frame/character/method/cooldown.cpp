#include "frame/global/cooldown.h"
#include "frame/character/character.h"
#include "frame/event.h"

using namespace jx3calc;
using namespace frame;

static event_tick_t s_calc_duration(Character *self, const frame::Cooldown &cooldown) {
    int durationFrame = cooldown.DurationFrame * 1024 / (1024 + self->chAttr.getHaste());
    durationFrame     = durationFrame > cooldown.MinDurationFrame ? durationFrame : cooldown.MinDurationFrame;
    durationFrame     = durationFrame < cooldown.MaxDurationFrame ? durationFrame : cooldown.MaxDurationFrame;
    return durationFrame * 1024 / 16;
}

static void cb_cooldown_modify(void *selfPtr, void *cooldownID) {
    // 利用 void * 传递一个 int 值, 避免内存的申请与回收.
    Character               *self     = (Character *)selfPtr;
    int                      ID       = static_cast<int>(reinterpret_cast<intptr_t>(cooldownID));
    const frame::Cooldown   &cooldown = CooldownManager::get(ID); // Global
    frame::ChCooldown::Item &item     = self->chCooldown.cooldownList[ID];

    item.countAvailable += 1;

    // 充能技能处理
    if (item.countAvailable < cooldown.MaxCount) {
        event_tick_t duration = s_calc_duration(self, cooldown); // 计算 duration

        // 如果 tickOverAdd 更短, 则使用 tickOverAdd.
        // 正常情况下, tickOverAdd 应当是 duration 的整数倍, 并且应该等于
        // (cooldown.MaxCount - item.countAvailable) * duration.
        // 除非本次计算的 duration 相比上次产生了变动 (比如 CD 受益于加速, 且调用者的加速属性有变化).
        // 截至目前 (2024年5月), 剑三不存在这种情况, 即 CD 受益于加速的同时是一个充能技能.
        duration = duration > item.tickOverAdd ? item.tickOverAdd : duration;

        item.tickOverCurr = Event::add(duration, cb_cooldown_modify, self, cooldownID);
        item.tickOverAdd -= duration;
        // } else {
        //     item.tickOverAdd = 0;
        // 不需要处理, 因为充能技能在上一次被该函数调用时, 上文中的
        // item.tickOverAdd -= duration;
        // 这一语句一定会将 item.tickOverAdd 置为 0 .
    }
}

static frame::ChCooldown::Item *s_get_or_create(Character *self, const frame::Cooldown &cooldown) {
    if (self->chCooldown.cooldownList.contains(cooldown.ID)) {
        return &self->chCooldown.cooldownList[cooldown.ID];
    } else {
        frame::ChCooldown::Item *res = &self->chCooldown.cooldownList[cooldown.ID];
        res->countAvailable          = cooldown.MaxCount;
        return res;
    }
}

void Character::cooldownClearTime(int cooldownID) {
    if (this->chCooldown.cooldownList.find(cooldownID) == this->chCooldown.cooldownList.end()) {
        return;
    }
    ChCooldown::Item      &item     = this->chCooldown.cooldownList.at(cooldownID);
    const frame::Cooldown &cooldown = CooldownManager::get(cooldownID); // Global
    if (item.countAvailable < cooldown.MaxCount) {
        Event::cancel(item.tickOverCurr, cb_cooldown_modify, this, reinterpret_cast<void *>(static_cast<intptr_t>(cooldown.ID)));
        item.countAvailable = cooldown.MaxCount;
        item.tickOverAdd    = 0;
    }
}

auto Character::cooldownLeft(int cooldownID) -> event_tick_t {
    if (!chCooldown.cooldownList.contains(cooldownID) ||
        chCooldown.cooldownList.at(cooldownID).countAvailable > 0) {
        return 0;
    }
    return chCooldown.cooldownList.at(cooldownID).tickOverCurr - Event::now();
}

void Character::cooldownModify(int cooldownID, int frame, int count) {
    if (frame == 0 && count == 0) {
        return;
    }
    // 获取资源
    const frame::Cooldown   &cooldown = CooldownManager::get(cooldownID); // Global
    frame::ChCooldown::Item *item     = s_get_or_create(this, cooldown);
    event_tick_t             delay    = 0;

    // 若该 CD 正在冷却, 则取消 Event 并获取距离当前层冷却完毕的剩余时间
    if (item->countAvailable < cooldown.MaxCount)
        delay = Event::cancel(item->tickOverCurr, cb_cooldown_modify, this, reinterpret_cast<void *>(static_cast<intptr_t>(cooldown.ID)));
    // 如果技能没有在冷却, 那么可以认为是离满层还差 1 层, 并且还有 0 tick 的时间就冷却好了
    else
        item->countAvailable--;

    event_tick_t duration = s_calc_duration(this, cooldown);
    event_tick_t delayAll = delay + item->tickOverAdd + frame * 1024 / 16 + count * duration;
    // 当前层的剩余冷却时间 + 其他层数的总冷却时间 + 本次修改的冷却时间
    if (delayAll <= 0) {
        item->countAvailable = cooldown.MaxCount;
        item->tickOverAdd    = 0;
        return;
    }
    int cnt = static_cast<int>(delayAll / duration) + 1; // 与满层的层数差. 至少是 1
    delay   = delayAll % duration;                       // 当前层的剩余冷却时间
    if (cnt > cooldown.MaxCount) {
        // 如果与满层的层数差已经超出了最大可用次数
        delay += (cnt - cooldown.MaxCount) * duration; // 超出部分退还为当前层的剩余冷却时间
        cnt = cooldown.MaxCount;                       // 修正为最大可用次数
    }
    item->countAvailable = cooldown.MaxCount - cnt;
    item->tickOverAdd    = delayAll - delay;
    item->tickOverCurr   = Event::add(delay, cb_cooldown_modify, this, reinterpret_cast<void *>(static_cast<intptr_t>(cooldown.ID)));
}

void Character::cooldownModify(int cooldownID, int frame) {
    cooldownModify(cooldownID, frame, 0);
}

void Character::cooldownReset(int cooldownID) {
    const frame::Cooldown   &cooldown = CooldownManager::get(cooldownID); // Global
    frame::ChCooldown::Item *item     = s_get_or_create(this, cooldown);
    if (item->countAvailable < cooldown.MaxCount) {
        // 若该 CD 正在冷却, 则取消 Event
        Event::cancel(item->tickOverCurr, cb_cooldown_modify, this, reinterpret_cast<void *>(static_cast<intptr_t>(cooldown.ID)));
    }
    item->countAvailable  = 0;
    event_tick_t duration = s_calc_duration(this, cooldown);
    item->tickOverAdd     = duration * (cooldown.MaxCount - 1);
    item->tickOverCurr    = Event::add(duration, cb_cooldown_modify, this, reinterpret_cast<void *>(static_cast<intptr_t>(cooldown.ID)));
}
