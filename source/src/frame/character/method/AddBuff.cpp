#include "frame/character/character.h"
#include "frame/character/helper/auto_rollback_attrib.h"
#include "frame/event.h"
#include "frame/global/buff.h"
#include "frame/global/skill.h"

using namespace ns_frame;

static void callbackActiveBuff(void *selfPtr, void *param) {
    Character *self = (Character *)selfPtr;
    CharacterBuff::Item *it = (CharacterBuff::Item *)param;
    ((AutoRollbackAttrib *)it->ptrAttrib)->active(); // ActivateAttrib
    (it->nLeftActiveCount)--;
    if (it->nLeftActiveCount <= 0) {
        self->buffDelAllStackNum(*it);
    } else {
        // 防止在回调函数中被删除, 需要判断其是否存在
        if (it->isValid) {
            it->tickActive = Event::add(it->interval * 1024 / 16, callbackActiveBuff, self, it); // 重新注册回调函数
        }
    }
}

void Character::AddBuff4(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel) {
    AddBuff7(buffSourceID, buffSourceLevel, buffID, buffLevel, 1, 0, 1);
}
void Character::AddBuff5(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int count) {
    AddBuff7(buffSourceID, buffSourceLevel, buffID, buffLevel, count, 0, 1);
}
void Character::AddBuff7(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int count, int param6, int stacknum) {
    bool newBuff = false;
    const Buff &buff = BuffManager::get(buffID, buffLevel);
    if (this->chBuff.buffMap[buffSourceID][buffID].find(buffLevel) == this->chBuff.buffMap[buffSourceID][buffID].end()) {
        // 对于 buffSourceID 和 buffID, 直接使用 [] 运算符, 没有则直接创建.
        this->chBuff.buffMap[buffSourceID][buffID].emplace(
            std::piecewise_construct,
            std::forward_as_tuple(buffLevel),
            std::forward_as_tuple(this->dwID, buffSourceID, buffID, buffLevel, static_cast<int>(this->chBuff.buffList.size()), buff.Interval, buff.Count));
        // 原地插入. 通过这种方式插入的 key 和 value, 不是构造后移动至容器, 而是直接在容器内构造. 这可以避免 Item 的 const 属性出现问题.
        newBuff = true;
    }
    CharacterBuff::Item &it = this->chBuff.buffMap[buffSourceID][buffID].at(buffLevel);
    if (newBuff)
        this->chBuff.buffList.emplace_back(&it);
    if (!it.isValid) {
        // 当前不存在 buff
        it.isValid = true;
        it.attr = characterGet(buffSourceID)->chAttr;           // 调用复制构造函数, 锁面板
        it.ptrAttrib = new AutoRollbackAttrib(this, &it, buff); // Attrib, 同时 new 调起构造函数, 自动处理 BeginAttrib
        it.nLeftActiveCount = buff.Count * count;
        // 计算 interval
        it.interval = buff.Interval * (1024 - it.attr.getHaste()) / 1024;
        it.interval = it.interval > buff.MaxInterval ? buff.MaxInterval : it.interval;
        it.interval = it.interval < buff.MinInterval ? buff.MinInterval : it.interval;
        // 注册回调函数
        it.tickActive = Event::add(it.interval * 1024 / 16, callbackActiveBuff, this, &it);
        // 其他工作
        it.nStackNum = stacknum; // 将层数设置为 1
    } else {
        // 当前存在该 buff
        it.attr = characterGet(buffSourceID)->chAttr; // 锁面板
        it.nLeftActiveCount = buff.Count * count;     // 重置计数
        // 重新计算 interval
        it.interval = buff.Interval * (1024 - it.attr.getHaste()) / 1024;
        it.interval = it.interval > buff.MaxInterval ? buff.MaxInterval : it.interval;
        it.interval = it.interval < buff.MinInterval ? buff.MinInterval : it.interval;
        // 其他工作
        if (buff.IsStackable)
            it.nStackNum += stacknum; // 层数 +stacknum
        if (buff.Exclude) {
            // Exclude 为 1 (true), 则重置下次生效时间 (通常用于常规 buff)
            Event::cancel(it.tickActive, callbackActiveBuff, this, &it);                        // 取出回调函数
            it.tickActive = Event::add(it.interval * 1024 / 16, callbackActiveBuff, this, &it); // 重新注册回调函数
            // else: Exclude 为 0 (false), 则不重置下次生效时间 (通常用于 DOT/HOT), 无需处理.
        }
    }
}

void Character::DelBuff(int buffID, int buffLevel) {
    CharacterBuff::Item *ptr = GetBuff(buffID, buffLevel);
    // 返回的一定是 isValie == true 的 Item.
    if (nullptr != ptr) {
        if (ptr->nStackNum > 1) {
            // 层数大于 1, 则层数 -1
            ptr->nStackNum--;
        } else {
            // 层数等于 1, 则删除该 buff
            buffDelAllStackNum(*ptr);
        }
    }
}

void Character::buffDelAllStackNum(CharacterBuff::Item &it) {
    it.isValid = false;
    delete (AutoRollbackAttrib *)it.ptrAttrib; // delete 调起析构函数, 自动回滚 BeginAttrib, 并处理 EndTimeAttrib
    it.ptrAttrib = nullptr;
    Event::cancel(it.tickActive, callbackActiveBuff, this, &it); // 取出回调函数
}

void Character::DelGroupBuff(int buffID, int buffLevel) {
    CharacterBuff::Item *ptr = GetBuff(buffID, buffLevel);
    // 返回的一定是 isValie == true 的 Item.
    if (nullptr != ptr) {
        buffDelAllStackNum(*ptr);
    }
}

void Character::DelMultiGroupBuffByID(int buffID) {
    CharacterBuff::Item *ptr = GetBuff(buffID, 0);
    while (nullptr != ptr) {
        buffDelAllStackNum(*ptr);
        ptr = GetBuff(buffID, 0);
    }
}

void Character::buffBind(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int skillID, int skillLevel) {
    const Skill &skill = SkillManager::get(skillID, skillLevel);
    const Buff &buff = BuffManager::get(buffID, buffLevel);
    this->AddBuff4(buffSourceID, buffSourceLevel, buffID, buffLevel);
    CharacterBuff::Item &it = this->chBuff.buffMap[buffSourceID][buffID].at(buffLevel);
    it.dwCasterSkillID = skillID;
    it.dwCasterSkillLevel = skillLevel;
    it.nChannelInterval = static_cast<int>(skill.nChannelInterval);
}

void Character::SetBuffLeftActiveCount(int buffIndex, int count) {
    if (buffIndex < 0 || buffIndex >= this->chBuff.buffList.size()) {
        return;
    }
    CharacterBuff::Item *buff = this->chBuff.buffList.at(buffIndex);
    buff->nLeftActiveCount = count;
}
void Character::SetBuffNextActiveFrame(int buffIndex, int nextActiveFrame) {
    if (buffIndex < 0 || buffIndex >= this->chBuff.buffList.size()) {
        return;
    }
    CharacterBuff::Item *buff = this->chBuff.buffList.at(buffIndex);
    Event::cancel(buff->tickActive, callbackActiveBuff, this, buff);
    buff->tickActive = Event::add(nextActiveFrame * 1024 / 16, callbackActiveBuff, this, buff);
}