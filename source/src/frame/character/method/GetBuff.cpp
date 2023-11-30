#include "frame/character/character.h"
#include "frame/static_ref.h"

using namespace ns_frame;

bool Character::IsHaveBuff(int buffID, int buffLevel) {
    return nullptr != GetBuff(buffID, buffLevel);
}

CharacterBuff::Item *Character::GetBuff(int buffID, int buffLevel) {
    if (0 == buffLevel) {
        return GetBuffWithCompareFlag(buffID, buffLevel + 1, static_cast<int>(ns_framestatic::enumLuaBuffCompareFlag::GREATER_EQUAL));
    } else {
        return GetBuffWithCompareFlag(buffID, buffLevel, static_cast<int>(ns_framestatic::enumLuaBuffCompareFlag::EQUAL));
    }
}

CharacterBuff::Item *Character::GetBuffByOwner(int buffID, int buffLevel, int sourceID) {
    if (0 == buffLevel) {
        return GetBuffByOwnerWithCompareFlag(buffID, buffLevel + 1, sourceID, static_cast<int>(ns_framestatic::enumLuaBuffCompareFlag::GREATER_EQUAL));
    } else {
        return GetBuffByOwnerWithCompareFlag(buffID, buffLevel, sourceID, static_cast<int>(ns_framestatic::enumLuaBuffCompareFlag::EQUAL));
    }
}

CharacterBuff::Item *Character::GetBuffWithCompareFlag(int buffID, int buffLevel, int flag) {
    for (auto &list : this->chBuff.buffList) { // 遍历 sourceID
        if (list.second.find(buffID) == list.second.end()) {
            continue; // 找不到有效的 buffID, 继续遍历 sourceID
        }
        // 没有 continue 说明 buffID 存在
        switch (flag) {
        case static_cast<int>(ns_framestatic::enumLuaBuffCompareFlag::EQUAL):
            if (list.second.at(buffID).find(buffLevel) != list.second.at(buffID).end() &&
                list.second.at(buffID).at(buffLevel).isValid) {
                return &(list.second.at(buffID).at(buffLevel));
            }
            break;
        case static_cast<int>(ns_framestatic::enumLuaBuffCompareFlag::GREATER):
            buffLevel += 1;
            // 注意, 这里不进行 break, 而是基于 +1 后的 buffLevel 继续执行 GREATER_EQUAL 的逻辑
        case static_cast<int>(ns_framestatic::enumLuaBuffCompareFlag::GREATER_EQUAL):
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

CharacterBuff::Item *Character::GetBuffByOwnerWithCompareFlag(int buffID, int buffLevel, int sourceID, int flag) {
    if (this->chBuff.buffList.find(sourceID) == this->chBuff.buffList.end()) {
        return nullptr; // 找不到有效的 sourceID
    }
    // 没有 return 说明 sourceID 存在
    if (this->chBuff.buffList.at(sourceID).find(buffID) == this->chBuff.buffList.at(sourceID).end()) {
        return nullptr; // 找不到有效的 buffID
    }
    // 没有 return 说明 sourceID 和 buffID 都存在
    switch (flag) {
    case static_cast<int>(ns_framestatic::enumLuaBuffCompareFlag::EQUAL):
        if (this->chBuff.buffList.at(sourceID).at(buffID).find(buffLevel) !=
                this->chBuff.buffList.at(sourceID).at(buffID).end() &&
            this->chBuff.buffList.at(sourceID).at(buffID).at(buffLevel).isValid) {
            return &(chBuff.buffList.at(sourceID).at(buffID).at(buffLevel));
        }
        break;
    case static_cast<int>(ns_framestatic::enumLuaBuffCompareFlag::GREATER):
        buffLevel += 1;
        // 注意, 这里不进行 break, 而是基于 +1 后的 buffLevel 继续执行 GREATER_EQUAL 的逻辑
    case static_cast<int>(ns_framestatic::enumLuaBuffCompareFlag::GREATER_EQUAL):
        auto it = this->chBuff.buffList.at(sourceID).at(buffID).lower_bound(buffLevel);
        while (it != this->chBuff.buffList.at(sourceID).at(buffID).end()) {
            if (it->second.isValid) {
                return &(it->second);
            }
            it++;
        }
        break;
    }
    return nullptr;
}