#include "frame/global/buff.h"
#include "frame/character/character.h"
#include "frame/character/helper/auto_rollback_attrib.h"
#include "frame/character/property/buff.h"
#include "frame/event.h"
#include "frame/global/skill.h"

#define UNREFERENCED_PARAMETER(P) (P)

using namespace jx3calc;
using namespace frame;

static void staticDelBuff(Character *self, BuffItem *it);

static void callbackActiveBuff(void *selfPtr, void *param) {
    Character *self = (Character *)selfPtr;
    BuffItem  *it   = (BuffItem *)param;
    static_cast<AutoRollbackAttrib *>(it->ptrAttrib)->active(); // ActivateAttrib
    if (it->nLeftActiveCount <= 1) {
        staticDelBuff(self, it);
    } else {
        // 防止在 activeAttrib 中被删除, 需要判断其是否存在
        if (it->isValid) {
            // 如果存在, 则重新注册回调函数
            it->tickActive = Event::add(it->interval * 1024 / 16, callbackActiveBuff, self, it);
        }
    }
    (it->nLeftActiveCount)--;
}

static void staticDelBuff(Character *self, BuffItem *it) {
    it->isValid = false;
    delete static_cast<AutoRollbackAttrib *>(it->ptrAttrib); // delete 调起析构函数, 自动回滚 BeginAttrib, 并处理 EndTimeAttrib
    self->autoRollbackAttribList.erase(static_cast<AutoRollbackAttrib *>(it->ptrAttrib));
    it->ptrAttrib = nullptr;
    Event::cancel(it->tickActive, callbackActiveBuff, self, it); // 取出回调函数
}

void Character::buffAddOptional(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, std::optional<int> count, std::optional<int> param6, std::optional<int> stacknum) {
    buffAdd(buffSourceID, buffSourceLevel, buffID, buffLevel, count.value_or(1), param6.value_or(0), stacknum.value_or(1));
}

void Character::buffAdd(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int count, int param6, int stacknum) {
    UNREFERENCED_PARAMETER(buffSourceLevel);
    UNREFERENCED_PARAMETER(param6);
    bool        newBuff = false;
    const Buff &buff    = BuffManager::get(buffID, buffLevel);
    if (this->chBuff.buffMap[buffSourceID][buffID].find(buffLevel) == this->chBuff.buffMap[buffSourceID][buffID].end()) {
        // 对于 buffSourceID 和 buffID, 直接使用 [] 运算符, 没有则直接创建.
        auto it = this->chBuff.buffMap[buffSourceID][buffID].emplace(
            std::piecewise_construct,
            std::forward_as_tuple(buffLevel),
            std::forward_as_tuple(this->dwID, buff.Interval, buff.Count, buffID, buffLevel, static_cast<int>(this->chBuff.buffList.size()), buffSourceID)
        );
        // 原地插入. 通过这种方式插入的 key 和 value, 不是构造后移动至容器, 而是直接在容器内构造. 这可以避免 Item 的 const 属性出现问题.
        this->chBuff.buffRef.emplace(buff.Name, &(it.first->second));
        newBuff = true;
    }
    BuffItem &it = this->chBuff.buffMap[buffSourceID][buffID].at(buffLevel);
    if (newBuff)
        this->chBuff.buffList.emplace_back(&it);
    if (!it.isValid) {
        // 当前不存在 buff
        it.isValid     = true;
        Character *src = characterGet(buffSourceID);
        if (nullptr != src) {      // lua 中有 AddBuff 时直接填 CharacterID 为 0 的逆天逻辑
            it.attr = src->chAttr; // 调用复制构造函数, 锁面板
        }
        it.nLeftActiveCount = buff.Count * count;
        // 计算 interval
        it.interval         = buff.Interval * 1024 / (1024 + it.attr.getHaste());
        it.interval         = it.interval > buff.MaxInterval ? buff.MaxInterval : it.interval;
        it.interval         = it.interval < buff.MinInterval ? buff.MinInterval : it.interval;
        // 清空 customValue
        it.nCustomValue     = 0;
        // 注册回调函数
        it.tickActive       = Event::add(it.interval * 1024 / 16, callbackActiveBuff, this, &it);
        // 其他工作
        if (buff.IsStackable) {
            it.nStackNum = stacknum > buff.MaxStackNum ? buff.MaxStackNum : stacknum; // 层数
        } else {
            it.nStackNum = 1; // 应该不会有 0 层的 AddBuff ?
        }
        // 创建 AutoRollbackAttrib, 处理 BeginAttrib
        it.ptrAttrib = new AutoRollbackAttrib(this, &it, buff); // Attrib, 同时 new 调起构造函数, 自动处理 BeginAttrib
        this->autoRollbackAttribList.emplace(static_cast<AutoRollbackAttrib *>(it.ptrAttrib));
    } else {
        // 当前存在该 buff
        // it.attr             = characterGet(buffSourceID)->chAttr; // 锁面板
        Character *src = characterGet(buffSourceID);
        if (nullptr != src) {      // lua 中有 AddBuff 时直接填 CharacterID 为 0 的逆天逻辑
            it.attr = src->chAttr; // 调用复制构造函数, 锁面板
        }
        it.nLeftActiveCount = buff.Count * count; // 重置计数
        // 重新计算 interval
        it.interval         = buff.Interval * 1024 / (1024 + it.attr.getHaste());
        it.interval         = it.interval > buff.MaxInterval ? buff.MaxInterval : it.interval;
        it.interval         = it.interval < buff.MinInterval ? buff.MinInterval : it.interval;
        // 其他工作
        if (buff.IsStackable) {
            AutoRollbackAttrib *ptr = static_cast<AutoRollbackAttrib *>(it.ptrAttrib);
            ptr->unload(); // UnloadAttrib
            it.nStackNum += stacknum;
            it.nStackNum = it.nStackNum > buff.MaxStackNum ? buff.MaxStackNum : it.nStackNum;
            ptr->load(); // LoadAttrib
        }
        if (buff.Exclude) {
            // Exclude 为 1 (true), 则重置下次生效时间 (通常用于常规 buff)
            Event::cancel(it.tickActive, callbackActiveBuff, this, &it);                        // 取出回调函数
            it.tickActive = Event::add(it.interval * 1024 / 16, callbackActiveBuff, this, &it); // 重新注册回调函数
            // else: Exclude 为 0 (false), 则不重置下次生效时间 (通常用于 DOT/HOT), 无需处理.
        }
    }
}

void Character::buffDel(int buffID, int buffLevel) {
    BuffItem *ptr = buffGet(buffID, buffLevel);
    // 返回的一定是 isValie == true 的 Item.
    if (nullptr != ptr) {
        if (ptr->nStackNum > 1) {
            // 层数大于 1, 则层数 -1
            ptr->nStackNum--;
        } else {
            // 层数等于 1, 则删除该 buff
            staticDelBuff(this, ptr);
        }
    }
}

void Character::buffDelGroup(int buffID, int buffLevel) {
    BuffItem *ptr = buffGet(buffID, buffLevel);
    // 返回的一定是 isValie == true 的 Item.
    if (nullptr != ptr) {
        staticDelBuff(this, ptr);
    }
}

void Character::buffDelMultiGroupByID(int buffID) {
    BuffItem *ptr = buffGet(buffID, 0);
    while (nullptr != ptr) {
        staticDelBuff(this, ptr);
        ptr = buffGet(buffID, 0);
    }
}

void Character::buffBind(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int skillID, int skillLevel) {
    const Skill &skill = SkillManager::get(skillID, skillLevel);
    // const Buff  &buff  = BuffManager::get(buffID, buffLevel);
    this->buffAdd(buffSourceID, buffSourceLevel, buffID, buffLevel);
    BuffItem &it          = this->chBuff.buffMap[buffSourceID][buffID].at(buffLevel);
    it.dwCasterSkillID    = skillID;
    it.dwCasterSkillLevel = skillLevel;
    it.nChannelInterval   = static_cast<int>(skill.nChannelInterval);
}

bool Character::buffExist(int buffID, int buffLevel) {
    return nullptr != buffGet(buffID, buffLevel);
}

void Character::buffFlushLeftFrame(BuffItem *item) {
    item->nLeftFrame       = item->interval * (item->nLeftActiveCount - 1) + static_cast<int>(item->tickActive - Event::now() + 63) / 64;
    item->nNextActiveFrame = static_cast<int>(item->tickActive - Event::now() + 63) / 64;
}

BuffItem *Character::buffGetWithCompareFlag(int buffID, int buffLevel, int flag) {
    for (auto &list : this->chBuff.buffMap) { // 遍历 sourceID
        if (list.second.find(buffID) == list.second.end()) {
            continue; // 找不到有效的 buffID, 继续遍历 sourceID
        }
        // 没有 continue 说明 buffID 存在
        switch (flag) {
        case static_cast<int>(ref::lua::BUFF_COMPARE_FLAG::EQUAL):
            if (list.second.at(buffID).find(buffLevel) != list.second.at(buffID).end() &&
                list.second.at(buffID).at(buffLevel).isValid) {
                return &(list.second.at(buffID).at(buffLevel));
            }
            break;
        case static_cast<int>(ref::lua::BUFF_COMPARE_FLAG::GREATER):
            buffLevel += 1;
            // 注意, 这里不进行 break, 而是基于 +1 后的 buffLevel 继续执行 GREATER_EQUAL 的逻辑
        case static_cast<int>(ref::lua::BUFF_COMPARE_FLAG::GREATER_EQUAL):
            auto it = list.second.at(buffID).lower_bound(buffLevel);
            while (it != list.second.at(buffID).end()) {
                if (it->second.isValid) {
                    return &(it->second);
                }
                it++;
            }
            break;
        }
    }
    return nullptr;
}

BuffItem *Character::buffGetByOwnerWithCompareFlag(int buffID, int buffLevel, int sourceID, int flag) {
    if (this->chBuff.buffMap.find(sourceID) == this->chBuff.buffMap.end()) {
        return nullptr; // 找不到有效的 sourceID
    }
    // 没有 return 说明 sourceID 存在
    if (this->chBuff.buffMap.at(sourceID).find(buffID) == this->chBuff.buffMap.at(sourceID).end()) {
        return nullptr; // 找不到有效的 buffID
    }
    // 没有 return 说明 sourceID 和 buffID 都存在
    switch (flag) {
    case static_cast<int>(ref::lua::BUFF_COMPARE_FLAG::EQUAL):
        if (this->chBuff.buffMap.at(sourceID).at(buffID).find(buffLevel) !=
                this->chBuff.buffMap.at(sourceID).at(buffID).end() &&
            this->chBuff.buffMap.at(sourceID).at(buffID).at(buffLevel).isValid) {
            return &(chBuff.buffMap.at(sourceID).at(buffID).at(buffLevel));
        }
        break;
    case static_cast<int>(ref::lua::BUFF_COMPARE_FLAG::GREATER):
        buffLevel += 1;
        // 注意, 这里不进行 break, 而是基于 +1 后的 buffLevel 继续执行 GREATER_EQUAL 的逻辑
    case static_cast<int>(ref::lua::BUFF_COMPARE_FLAG::GREATER_EQUAL):
        auto it = this->chBuff.buffMap.at(sourceID).at(buffID).lower_bound(buffLevel);
        while (it != this->chBuff.buffMap.at(sourceID).at(buffID).end()) {
            if (it->second.isValid) {
                return &(it->second);
            }
            it++;
        }
        break;
    }
    return nullptr;
}

BuffItem *Character::buffGet(int buffID, int buffLevel) {
    BuffItem *ret = nullptr;
    if (0 == buffLevel) {
        ret = buffGetWithCompareFlag(buffID, buffLevel + 1, static_cast<int>(ref::lua::BUFF_COMPARE_FLAG::GREATER_EQUAL));
    } else {
        ret = buffGetWithCompareFlag(buffID, buffLevel, static_cast<int>(ref::lua::BUFF_COMPARE_FLAG::EQUAL));
    }
    if (ret != nullptr) {
        this->buffFlushLeftFrame(ret);
    }
    return ret;
}

BuffItem *Character::buffGetByOwner(int buffID, int buffLevel, int sourceID) {
    BuffItem *ret = nullptr;
    if (0 == buffLevel) {
        ret = buffGetByOwnerWithCompareFlag(buffID, buffLevel + 1, sourceID, static_cast<int>(ref::lua::BUFF_COMPARE_FLAG::GREATER_EQUAL));
    } else {
        ret = buffGetByOwnerWithCompareFlag(buffID, buffLevel, sourceID, static_cast<int>(ref::lua::BUFF_COMPARE_FLAG::EQUAL));
    }
    if (ret != nullptr) {
        this->buffFlushLeftFrame(ret);
    }
    return ret;
}

void Character::buffSetLeftActiveCount(int buffIndex, int count) {
    if (buffIndex < 0 || buffIndex >= static_cast<int>(this->chBuff.buffList.size())) {
        return;
    }
    BuffItem *buff         = this->chBuff.buffList.at(buffIndex);
    buff->nLeftActiveCount = count;
}
void Character::buffSetNextActiveFrame(int buffIndex, int nextActiveFrame) {
    if (buffIndex < 0 || buffIndex >= static_cast<int>(this->chBuff.buffList.size())) {
        return;
    }
    BuffItem *buff = this->chBuff.buffList.at(buffIndex);
    Event::cancel(buff->tickActive, callbackActiveBuff, this, buff);
    buff->tickActive = Event::add(nextActiveFrame * 1024 / 16, callbackActiveBuff, this, buff);
}

event_tick_t Character::buffTimeLeftTick(int buffID, int buffLevel) {
    BuffItem *ret = buffGet(buffID, buffLevel);
    if (ret != nullptr) {
        return ret->nLeftFrame * 1024 / 16;
    }
    return 0;
}
