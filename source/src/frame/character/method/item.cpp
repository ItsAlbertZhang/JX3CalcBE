#include "frame/global/item.h"
#include "frame/character/character.h"
#include "frame/global/cooldown.h"
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
        if (chCooldown.cooldownList.contains(item.CoolDownID)) {
            const ChCooldown::Item &cd = chCooldown.cooldownList.at(item.CoolDownID);
            if (cd.countAvailable == 0) {
                return;
            }
        }
        // 触发 CD
        const Cooldown &cooldown      = CooldownManager::get(item.CoolDownID);
        int             durationFrame = cooldown.DurationFrame * 1024 / (1024 + chAttr.getHaste());
        durationFrame                 = durationFrame > cooldown.MinDurationFrame ? durationFrame : cooldown.MinDurationFrame;
        durationFrame                 = durationFrame < cooldown.MaxDurationFrame ? durationFrame : cooldown.MaxDurationFrame;
        cooldownModify(item.CoolDownID, durationFrame);
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
