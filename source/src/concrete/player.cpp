#include "concrete.h"

using namespace jx3calc;

auto concrete::createPlayer(PlayerType type) -> std::unique_ptr<frame::Player> {
    const std::unordered_map<PlayerType, std::unordered_map<modules::config::ClientType, std::function<std::unique_ptr<frame::Player>()>>> funcmap {
        {PlayerType::MjFysj,
         {
             {modules::config::ClientType::jx3_hd, []() { return createPlayer<PlayerType::MjFysj, modules::config::ClientType::jx3_hd>(); }},
             {modules::config::ClientType::jx3_exp, []() { return createPlayer<PlayerType::MjFysj, modules::config::ClientType::jx3_exp>(); }},
         }},
    };
    return funcmap.at(type).at(modules::config::clientType)();
}
