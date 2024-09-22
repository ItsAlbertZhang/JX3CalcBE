#pragma once

#include "frame/character/character.h"

namespace jx3calc {
namespace frame {

namespace effect {

struct tBuff {
    int    id       = 0;
    int    level    = 0;
    int    count    = 1;
    int    stacknum = 1;
    double covrate  = 0;
    bool   target   = false;

    void (*add)(frame::Character *) = nullptr;
};
struct tItem {
    frame::ItemType type;
    int             id;
};
struct tSkillevent {
    int id;
};
struct tSkillrecipe {
    int id;
    int level;
};

using anySingle   = std::variant<tBuff, tItem, tSkillevent, tSkillrecipe>;
using anyMultiple = std::vector<anySingle>;
using anyFunc     = void (*)(frame::Character *);
using any         = std::variant<anySingle, anyMultiple, anyFunc>;
using anyMap      = std::unordered_map<std::string, any>;

using typeMap = std::unordered_map<std::string, std::variant<any, anyMap>>;

} // namespace effect

class Effect {
public:
    Effect(effect::any value) :
        value(std::move(value)) {}
    void active(frame::Character *obj);

private:
    effect::any value;
};

namespace effect {
auto create(const effect::typeMap &effect_map, const std::string &type, const std::string &jsonstr) -> std::shared_ptr<frame::Effect>;
}

} // namespace frame
} // namespace jx3calc
