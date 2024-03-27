#pragma once

#include "frame/character/character.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace jx3calc {
namespace concrete {

namespace effect {

enum class Type {
    大附魔·腰,
    大附魔·腕,
    大附魔·鞋,
    套装·技能,
    套装·特效,
    腰坠·特效,
    武器·橙武,
    武器·水特效,
    家园酒·加速,
};

inline const std::unordered_map<std::string, Type> refType = {
    {"大附魔·腰",    Type::大附魔·腰   },
    {"大附魔·腕",    Type::大附魔·腕   },
    {"大附魔·鞋",    Type::大附魔·鞋   },
    {"套装·技能",    Type::套装·技能   },
    {"套装·特效",    Type::套装·特效   },
    {"腰坠·特效",    Type::腰坠·特效   },
    {"武器·橙武",    Type::武器·橙武   },
    {"武器·水特效", Type::武器·水特效},
    {"家园酒·加速", Type::家园酒·加速},
};

class Base {
public:
    virtual void active(frame::Character *obj) const = 0;
};

} // namespace effect

// template <effect::Type type>
// extern auto create() -> std::shared_ptr<effect::Base>;

auto create(effect::Type type) -> std::shared_ptr<effect::Base>;

} // namespace concrete
} // namespace jx3calc
