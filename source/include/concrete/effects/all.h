#ifndef CONCRETE_EFFECTS_ALL_H_
#define CONCRETE_EFFECTS_ALL_H_

#include "concrete/effects/base.h"
#include <memory>

namespace ns_concrete {

enum class EffectType {
    大附魔·腰,
    大附魔·腕,
    大附魔·鞋,
    套装·技能,
    套装·特效,
    武器·橙武,
    COUNT,
};

inline std::unordered_map<std::string, EffectType> EffectTypeMap = {
    {"大附魔·腰", EffectType::大附魔·腰},
    {"大附魔·腕", EffectType::大附魔·腕},
    {"大附魔·鞋", EffectType::大附魔·鞋},
    {"套装·技能", EffectType::套装·技能},
    {"套装·特效", EffectType::套装·特效},
    {"武器·橙武", EffectType::武器·橙武},
};

class EffectManager {
public:
    EffectManager() = delete;
    static std::shared_ptr<EffectBase> create(EffectType type);
};

} // namespace ns_concrete

#endif // CONCRETE_EFFECTS_ALL_H_
