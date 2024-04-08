#include "concrete/effect.h"
#include "frame/common/item.h"

using namespace jx3calc;
using namespace concrete;

namespace {

class Effect大附魔·腰 : public effect::Base {
public:
    virtual void active(frame::Character *obj) const override {
        obj->skilleventAdd(1705);
    }
};

class Effect大附魔·腕 : public effect::Base {
public:
    virtual void active(frame::Character *obj) const override {
        obj->skilleventAdd(1843);
    }
};

class Effect大附魔·鞋 : public effect::Base {
public:
    virtual void active(frame::Character *obj) const override {
        obj->skilleventAdd(2393);
    }
};

class Effect套装·技能 : public effect::Base {
public:
    virtual void active(frame::Character *obj) const override {
        obj->skillrecipeAdd(948, 2);
    }
};

class Effect套装·特效 : public effect::Base {
public:
    virtual void active(frame::Character *obj) const override {
        obj->skilleventAdd(1922);
    }
};

class Effect腰坠·特效 : public effect::Base {
public:
    virtual void active(frame::Character *obj) const override {
        switch (obj->kungfuID) {
        case 10242:
            // 焚影圣诀
            obj->itemAdd(frame::ItemType::Trinket, 38789); // 吹香雪
            break;
        default:
            break;
        }
    }
};

class Effect武器·橙武 : public effect::Base {
public:
    virtual void active(frame::Character *obj) const override {
        switch (obj->kungfuID) {
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
};

class Effect武器·水特效 : public effect::Base {
public:
    virtual void active(frame::Character *obj) const override {
        obj->chAttr.atPhysicsAttackPowerBase += 0; // 外功数值懒得找了, 后面如果做外功的水特效再补上
        obj->chAttr.atMagicAttackPowerBase += 1170;
    }
};

class Effect家园酒·加速 : public effect::Base {
public:
    virtual void active(frame::Character *obj) const override {
        obj->chAttr.atHasteBase += 286;
    }
};

} // namespace

auto jx3calc::concrete::create(effect::Type type) -> std::shared_ptr<effect::Base> {
    switch (type) {
    case effect::Type::大附魔·腰:
        return std::make_shared<Effect大附魔·腰>();
    case effect::Type::大附魔·腕:
        return std::make_shared<Effect大附魔·腕>();
    case effect::Type::大附魔·鞋:
        return std::make_shared<Effect大附魔·鞋>();
    case effect::Type::套装·技能:
        return std::make_shared<Effect套装·技能>();
    case effect::Type::套装·特效:
        return std::make_shared<Effect套装·特效>();
    case effect::Type::腰坠·特效:
        return std::make_shared<Effect腰坠·特效>();
    case effect::Type::武器·橙武:
        return std::make_shared<Effect武器·橙武>();
    case effect::Type::武器·水特效:
        return std::make_shared<Effect武器·水特效>();
    case effect::Type::家园酒·加速:
        return std::make_shared<Effect家园酒·加速>();
    }
}
