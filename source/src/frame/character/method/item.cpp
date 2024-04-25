#include "frame/global/item.h"
#include "frame/character/character.h"
#include <stdexcept>

using namespace jx3calc;
using namespace frame;

void Character::itemAdd(ItemType type, int ID) {
    if (type == ItemType::COUNT) {
        throw std::range_error("Invalid ItemType");
    }
    chItem[static_cast<size_t>(type)].insert(ID);
}

void Character::itemAdd(int type, int ID) {
    if (type >= 0 && type < static_cast<int>(ItemType::COUNT))
        itemAdd(static_cast<ItemType>(type), ID);
}

void Character::itemUse(ItemType type, int ID) {
    if (type == ItemType::COUNT) {
        throw std::range_error("Invalid ItemType");
    }
    if (!chItem[static_cast<int>(type)].contains(ID))
        return;
    const Item &item = ItemManager::get(type, ID);

    if (item.CoolDownID != 0) {
        // 检查 CD
        if (cooldownLeft(item.CoolDownID) > 0) {
            return;
        }
        // 触发 CD
        cooldownModify(item.CoolDownID, 0, 1);
    }

    // 触发技能
    if (item.SkillID != 0 && item.SkillLevel != 0) {
        skillCast(item.SkillID, item.SkillLevel);
    }
}

void Character::itemUse(int type, int ID) {
    if (type >= 0 && type < static_cast<int>(ItemType::COUNT))
        itemUse(static_cast<ItemType>(type), ID);
}
