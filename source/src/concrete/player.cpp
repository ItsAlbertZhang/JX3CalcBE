#include "frame/character/derived/player.h"
#include "concrete.h"

using namespace jx3calc;

auto concrete::createPlayer(Player type) -> std::unique_ptr<frame::Player> {
    switch (type) {
    case Player::MjFysj:
        return createPlayer<Player::MjFysj>();
    default:
        throw std::range_error("Invalid playerType");
    }
}
