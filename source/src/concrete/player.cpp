#include "concrete/player.h"

using namespace jx3calc;
using namespace concrete;

auto concrete::create(player::Type type, int delayNetwork, int delayKeyboard) -> std::unique_ptr<frame::Player> {
    switch (type) {
    case player::Type::MjFysj:
        return create<player::Type::MjFysj>(delayNetwork, delayKeyboard);
    default:
        throw std::range_error("Invalid playerType");
    }
}
