#ifndef FRAME_CHARACTER_PROPERTY_ITEM_H_
#define FRAME_CHARACTER_PROPERTY_ITEM_H_

#include "frame/common/item.h"
#include <array>
#include <set>

namespace jx3calc {
namespace frame {

using ChItem = std::array<std::set<int>, static_cast<size_t>(ItemType::COUNT)>;

} // namespace frame
} // namespace jx3calc

#endif // FRAME_CHARACTER_PROPERTY_ITEM_H_
