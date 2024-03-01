#include "concrete/effect/derived.h"

using namespace ns_concrete;

void Effect大附魔·腰::active(ns_frame::Character *obj) const {
    obj->skilleventAdd(1705);
}

void Effect大附魔·腕::active(ns_frame::Character *obj) const {
    obj->skilleventAdd(1843);
}

void Effect大附魔·鞋::active(ns_frame::Character *obj) const {
    obj->skilleventAdd(2393);
}

void Effect套装·技能::active(ns_frame::Character *obj) const {
    obj->skillrecipeAdd(948, 2);
}

void Effect套装·特效::active(ns_frame::Character *obj) const {
    obj->skilleventAdd(1922);
}

void Effect武器·橙武::active(ns_frame::Character *obj) const {
    obj->skilleventAdd(2421);
}

void Effect武器·水特效::active(ns_frame::Character *obj) const {
    obj->chAttr.atPhysicsAttackPowerBase += 0; // 外功数值懒得找了, 后面如果做外功的水特效再补上
    obj->chAttr.atMagicAttackPowerBase += 1170;
}

void Effect家园酒·加速::active(ns_frame::Character *obj) const {
    obj->chAttr.atHasteBase += 286;
}
