#include "concrete/effect/derived.h"
#include "frame/common/item.h"

using namespace jx3calc;
using namespace concrete;

void Effect大附魔·腰::active(frame::Character *obj) const {
    obj->skilleventAdd(1705);
}

void Effect大附魔·腕::active(frame::Character *obj) const {
    obj->skilleventAdd(1843);
}

void Effect大附魔·鞋::active(frame::Character *obj) const {
    obj->skilleventAdd(2393);
}

void Effect套装·技能::active(frame::Character *obj) const {
    obj->skillrecipeAdd(948, 2);
}

void Effect套装·特效::active(frame::Character *obj) const {
    obj->skilleventAdd(1922);
}

void Effect腰坠·特效::active(frame::Character *obj) const {
    switch (obj->dwKungfuID) {
    case 10242:
        // 焚影圣诀
        obj->itemAdd(frame::ItemType::Trinket, 38789); // 吹香雪
        break;
    default:
        break;
    }
}

void Effect武器·橙武::active(frame::Character *obj) const {
    switch (obj->dwKungfuID) {
    case 10242:
        // 焚影圣诀
        obj->skillrecipeAdd(1542, 1); // 驱夜伤害提高 5%
        obj->skillrecipeAdd(1543, 1); // 净世破魔击伤害提高 5%
        obj->skilleventAdd(2421);     // 触发特效
        obj->skilleventAdd(1938);     // 施展驱夜断愁造成少量伤害
        break;
    default:
        break;
    }
}

void Effect武器·水特效::active(frame::Character *obj) const {
    obj->chAttr.atPhysicsAttackPowerBase += 0; // 外功数值懒得找了, 后面如果做外功的水特效再补上
    obj->chAttr.atMagicAttackPowerBase += 1170;
}

void Effect家园酒·加速::active(frame::Character *obj) const {
    obj->chAttr.atHasteBase += 286;
}
