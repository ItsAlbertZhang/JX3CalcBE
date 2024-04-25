#include "concrete/player.h"

using namespace jx3calc;
using namespace concrete;

auto concrete::create(player::Type type) -> std::unique_ptr<frame::Player> {
    switch (type) {
    case player::Type::MjFysj:
        return create<player::Type::MjFysj>();
    default:
        throw std::range_error("Invalid playerType");
    }
}
