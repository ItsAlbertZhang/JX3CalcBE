#pragma once

#include "frame/common/item.h"
#include <array>
#include <set>

namespace jx3calc {
namespace frame {

using ChItem = std::array<std::set<int>, static_cast<size_t>(ItemType::COUNT)>;

} // namespace frame
} // namespace jx3calc
