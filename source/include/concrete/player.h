#pragma once

#include "frame/character/derived/player.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace jx3calc {
namespace concrete {

namespace player {

enum class Type {
    MjFysj,
};

inline const std::unordered_map<std::string, Type> refType = {
    {"焚影圣诀", Type::MjFysj},
};

} // namespace player

template <player::Type type>
extern auto create() -> std::unique_ptr<frame::Player>;

auto create(player::Type type) -> std::unique_ptr<frame::Player>;

} // namespace concrete
} // namespace jx3calc
