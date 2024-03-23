#ifndef CONCRETE_EFFECTS_ALL_H_
#define CONCRETE_EFFECTS_ALL_H_

#include "concrete/effect/base.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace jx3calc {
namespace concrete {

enum class EffectType {
    大附魔·腰,
    大附魔·腕,
    大附魔·鞋,
    套装·技能,
    套装·特效,
    腰坠·特效,
    武器·橙武,
    武器·水特效,
    家园酒·加速,
    COUNT,
};

inline const std::unordered_map<std::string, EffectType> refEffectType{
    {"大附魔·腰",    EffectType::大附魔·腰   },
    {"大附魔·腕",    EffectType::大附魔·腕   },
    {"大附魔·鞋",    EffectType::大附魔·鞋   },
    {"套装·技能",    EffectType::套装·技能   },
    {"套装·特效",    EffectType::套装·特效   },
    {"腰坠·特效",    EffectType::腰坠·特效   },
    {"武器·橙武",    EffectType::武器·橙武   },
    {"武器·水特效", EffectType::武器·水特效},
    {"家园酒·加速", EffectType::家园酒·加速},
};

std::shared_ptr<EffectBase> createEffect(EffectType type);

} // namespace concrete
} // namespace jx3calc

#endif // CONCRETE_EFFECTS_ALL_H_
