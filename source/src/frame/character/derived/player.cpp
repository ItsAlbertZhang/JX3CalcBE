#include "frame/character/derived/player.h"
#include "frame/event.h"
#include "program/log.h"
#include <random>

using namespace ns_frame;

Player::Player() {
    isPlayer = true;
}

static void callbackMacroDefault(void *self, void *nullparam);
static void callbackMacroCustom(void *self, void *nullparam);

void Player::macroRun() {
    if (macroCustom == nullptr) {
        macroPrepareDefault();
        callbackMacroDefault(this, nullptr);
    } else {
        macroCustom->macroPrepare(this);
        callbackMacroCustom(this, nullptr);
    }
}

inline static ns_frame::event_tick_t getDelay(Player *player) {
    ns_frame::event_tick_t delay = 0;
    if (player->delayCustom > 0) {
        // 优先使用自定义延迟
        delay               = player->delayCustom;
        player->delayCustom = 0;
    } else if (player->publicCooldownID > 0) {
        // 其次使用 GCD
        ns_frame::event_tick_t over = player->chCooldown.cooldownList.at(player->publicCooldownID).tickOver;
        ns_frame::event_tick_t now  = ns_frame::Event::now();
        delay                       = over > now ? over - now : 0; // 当前无技能可放, 将延迟设为 0.
    }
    // 加上网络延迟和按键延迟
    delay += player->delayBase;
    std::random_device              rd;
    std::mt19937                    gen(rd());
    std::uniform_int_distribution<> dis(0, player->delayRand);
    delay += dis(gen);
    return delay;
}

static void callbackMacroDefault(void *self, void *nullparam) {
    Player *player = static_cast<Player *>(self);
    player->macroRuntimeDefault();
    Event::add(getDelay(player), callbackMacroDefault, self, nullptr);
}

static void callbackMacroCustom(void *self, void *nullparam) {
    Player *player = static_cast<Player *>(self);
    if (player->macroIdx >= player->macroCustom->macroRuntime.size()) {
        LOG_ERROR("macroIdx >= macroRuntime.size(){}", "");
        return;
    }
    player->macroCustom->macroRuntime.at(player->macroIdx)(player);
    Event::add(getDelay(player), callbackMacroCustom, self, nullptr);
}
