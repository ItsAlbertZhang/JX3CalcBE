#pragma once

#include "frame/character/character.h"
#include <memory>
#include <string>

namespace jx3calc {

namespace frame {
class Effect {
public:
    virtual void active(frame::Character *obj) = 0;
};
} // namespace frame

namespace concrete {
auto createEffect(const std::string &type, const std::string &jsonstr) -> std::shared_ptr<frame::Effect>;
} // namespace concrete

} // namespace jx3calc
