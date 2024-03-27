#pragma once

#include "frame/character/derived/npc.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace jx3calc {
namespace concrete {

namespace npc {

enum class Type {
    NPC124,
};

inline const std::unordered_map<std::string, Type> refType = {
    {"124级NPC目标", npc::Type::NPC124},
};

} // namespace npc

// template <npc::Type type>
// extern auto create() -> std::unique_ptr<frame::NPC>;

auto create(npc::Type type) -> std::unique_ptr<frame::NPC>;

} // namespace concrete
} // namespace jx3calc
