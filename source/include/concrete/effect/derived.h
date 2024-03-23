#ifndef CONCRETE_EFFECTS_DERIVED_H_
#define CONCRETE_EFFECTS_DERIVED_H_

#include "concrete/effect/base.h"

namespace jx3calc {
namespace concrete {

class Effect大附魔·腰 : public EffectBase {
public:
    virtual void active(frame::Character *obj) const override;
};

class Effect大附魔·腕 : public EffectBase {
public:
    virtual void active(frame::Character *obj) const override;
};

class Effect大附魔·鞋 : public EffectBase {
public:
    virtual void active(frame::Character *obj) const override;
};

class Effect套装·技能 : public EffectBase {
public:
    virtual void active(frame::Character *obj) const override;
};

class Effect套装·特效 : public EffectBase {
public:
    virtual void active(frame::Character *obj) const override;
};

class Effect腰坠·特效 : public EffectBase {
public:
    virtual void active(frame::Character *obj) const override;
};

class Effect武器·橙武 : public EffectBase {
public:
    virtual void active(frame::Character *obj) const override;
};

class Effect武器·水特效 : public EffectBase {
public:
    virtual void active(frame::Character *obj) const override;
};

class Effect家园酒·加速 : public EffectBase {
public:
    virtual void active(frame::Character *obj) const override;
};

} // namespace concrete
} // namespace jx3calc

#endif // CONCRETE_EFFECTS_DERIVED_H_
