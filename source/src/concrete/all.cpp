#include "concrete/effect/derived.h"
#include "concrete/effect/effect.h"

using namespace ns_concrete;

std::shared_ptr<EffectBase> ns_concrete::createEffect(EffectType type) {
    switch (type) {
    case EffectType::大附魔·腰:
        return std::make_shared<Effect大附魔·腰>();
    case EffectType::大附魔·腕:
        return std::make_shared<Effect大附魔·腕>();
    case EffectType::大附魔·鞋:
        return std::make_shared<Effect大附魔·鞋>();
    case EffectType::套装·技能:
        return std::make_shared<Effect套装·技能>();
    case EffectType::套装·特效:
        return std::make_shared<Effect套装·特效>();
    // case EffectType::武器·橙武:
    //     return std::make_shared<Effect武器·橙武>();
    default:
        return nullptr;
    }
}
