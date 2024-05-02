#pragma once

#include "frame/character/character.h"
#include "nlohmann/json.hpp"
#include <memory>
#include <string>

namespace jx3calc {
namespace concrete {

namespace effect {

class Base {
public:
    virtual void active(frame::Character *obj) const = 0;
};

auto create(const std::string &type, const nlohmann::json &value) -> std::shared_ptr<effect::Base>;

} // namespace effect

} // namespace concrete
} // namespace jx3calc
