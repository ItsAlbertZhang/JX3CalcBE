#ifndef CONCRETE_EFFECTS_ALL_H_
#define CONCRETE_EFFECTS_ALL_H_

#include "concrete/effect/base.h"
#include <array>
#include <memory>
#include <string>

namespace ns_concrete {

enum class EffectType {
    大附魔·腰,
    大附魔·腕,
    大附魔·鞋,
    套装·技能,
    套装·特效,
    // 武器·橙武,
    COUNT,
};

inline const std::array<std::string, static_cast<size_t>(EffectType::COUNT)> refEffectType = {
    "大附魔·腰",
    "大附魔·腕",
    "大附魔·鞋",
    "套装·技能",
    "套装·特效",
    // "武器·橙武",
};

std::shared_ptr<EffectBase> createEffect(EffectType type);

} // namespace ns_concrete

#endif // CONCRETE_EFFECTS_ALL_H_
