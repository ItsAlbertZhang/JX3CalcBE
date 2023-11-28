#include "frame/character/character.h"
#include "frame/character/helper/auto_rollback_attrib.h"
#include "frame/event.h"
#include "frame/global/buff.h"

using namespace ns_frame;

static void callbackActiveBuff(void *selfPtr, void *param) {
    Character *self = (Character *)selfPtr;
    CharacterBuff::Item *it = (CharacterBuff::Item *)param;
    ((AutoRollbackAttrib *)it->ptrAttrib)->active(); // ActivateAttrib
    // TODO: 其他工作, 例如再次注册回调函数, 调整 Count 等
    (it->count)--;
    if (it->count <= 0) {
        self->DelBuffAllStackNum(*it);
    } else {
        it->tickActive = EventManager::add(it->interval * 1024 / 16, callbackActiveBuff, self, it); // 重新注册回调函数
    }
}

void Character::AddBuff(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel) {
    printf("%d AddBuff: %d %d %d %d\n", this->dwID, buffSourceID, buffSourceLevel, buffID, buffLevel);
    if (this->chBuff.buffList[buffSourceID][buffID].find(buffLevel) == this->chBuff.buffList[buffSourceID][buffID].end()) {
        // 对于 buffSourceID 和 buffID, 直接使用 [] 运算符, 没有则直接创建.
        this->chBuff.buffList[buffSourceID][buffID].emplace(
            std::piecewise_construct,
            std::forward_as_tuple(buffLevel),
            std::forward_as_tuple(this->dwID, buffSourceID, buffID, buffLevel));
        // 原地插入. 通过这种方式插入的 key 和 value, 不是构造后移动至容器, 而是直接在容器内构造. 这可以避免 Item 的 const 属性出现问题.
    }
    CharacterBuff::Item &it = this->chBuff.buffList[buffSourceID][buffID].at(buffLevel);
    const Buff &buff = BuffManager::get(buffID, buffLevel);
    if (!it.isValid) {
        // 当前不存在 buff
        it.isValid = true;
        it.attr = getCharacter(buffSourceID)->chAttr;                // 锁面板
        it.ptrAttrib = new AutoRollbackAttrib(this, &it.attr, buff); // Attrib, 同时 new 调起构造函数, 自动处理 BeginAttrib
        it.count = buff.Count;
        // 计算 interval
        it.interval = buff.Interval * (1024 - it.attr.getHaste()) / 1024;
        it.interval = it.interval > buff.MaxInterval ? buff.MaxInterval : it.interval;
        it.interval = it.interval < buff.MinInterval ? buff.MinInterval : it.interval;
        // 注册回调函数
        it.tickActive = EventManager::add(it.interval * 1024 / 16, callbackActiveBuff, this, &it);
        // 其他工作
        it.nStackNum = 1; // 将层数设置为 1
    } else {
        // 当前存在该 buff
        it.attr = getCharacter(buffSourceID)->chAttr;          // 锁面板
        ((AutoRollbackAttrib *)it.ptrAttrib)->attr = &it.attr; // 更换 Attrib 所使用的 attr
        it.count = buff.Count;                                 // 重置计数
        // 重新计算 interval
        it.interval = buff.Interval * (1024 - it.attr.getHaste()) / 1024;
        it.interval = it.interval > buff.MaxInterval ? buff.MaxInterval : it.interval;
        it.interval = it.interval < buff.MinInterval ? buff.MinInterval : it.interval;
        // 其他工作
        if (buff.IsStackable)
            it.nStackNum++; // 层数 +1
        if (buff.Exclude) {
            // Exclude 为 1 (true), 则重置下次生效时间 (通常用于常规 buff)
            EventManager::cancel(it.tickActive, callbackActiveBuff, this, &it);                        // 取出回调函数
            it.tickActive = EventManager::add(it.interval * 1024 / 16, callbackActiveBuff, this, &it); // 重新注册回调函数
            // else: Exclude 为 0 (false), 则不重置下次生效时间 (通常用于 DOT/HOT), 无需处理.
        }
    }
}

void Character::DelBuffAllStackNum(CharacterBuff::Item &it) {
    it.isValid = false;
    delete (AutoRollbackAttrib *)it.ptrAttrib; // delete 调起析构函数, 自动回滚 BeginAttrib, 并处理 EndTimeAttrib
    it.ptrAttrib = nullptr;
    EventManager::cancel(it.tickActive, callbackActiveBuff, this, &it); // 取出回调函数
}