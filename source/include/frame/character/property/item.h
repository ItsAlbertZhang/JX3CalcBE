#ifndef FRAME_CHARACTER_PROPERTY_ITEM_H_
#define FRAME_CHARACTER_PROPERTY_ITEM_H_

#include "frame/common/item.h"
#include <array>
#include <set>

namespace ns_frame {

using ChItem = std::array<std::set<int>, static_cast<size_t>(ItemType::COUNT)>;

} // namespace ns_frame

#endif // FRAME_CHARACTER_PROPERTY_ITEM_H_
