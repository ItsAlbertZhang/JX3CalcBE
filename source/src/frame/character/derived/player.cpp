#include "frame/character/derived/player.h"
#include "frame/event.h"
#include "program/log.h"
#include <random>

#define UNREFERENCED_PARAMETER(P) (P)

using namespace ns_frame;

Player::Player(int delayNetwork, int delayKeybord)
    : Character(), delayBase(delayNetwork), delayRand(delayKeybord) {
    isPlayer = true;
}

static void callbackMacroDefault(void *self, void *nullparam);
static void callbackMacroCustom(void *self, void *nullparam);
static void callbackNormalAttack(void *self, void *nullparam);

void Player::macroRun() {
    if (macroCustom == nullptr) {
        macroPrepareDefault();               // 起手
        callbackMacroDefault(this, nullptr); // 进入战斗
        callbackNormalAttack(this, nullptr); // 开启普通攻击
    } else {
        macroCustom->macroPrepare(this);     // 起手
        callbackMacroCustom(this, nullptr);  // 进入战斗
        callbackNormalAttack(this, nullptr); // 开启普通攻击
    }
}

// 计算网络延迟和按键延迟
inline static ns_frame::event_tick_t getDelayAdd(Player *player) {
    std::random_device              rd;
    std::mt19937                    gen(rd());
    std::uniform_int_distribution<> dis(0, player->delayRand);
    return player->delayBase + dis(gen);
}

// 计算下一次技能延迟
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
    delay += getDelayAdd(player);
    return delay;
}

static void callbackMacroDefault(void *self, void *nullparam) {
    UNREFERENCED_PARAMETER(nullparam);
    Player *player = static_cast<Player *>(self);
    player->macroRuntimeDefault();
    Event::add(getDelay(player), callbackMacroDefault, self, nullptr);
}

static void callbackMacroCustom(void *self, void *nullparam) {
    UNREFERENCED_PARAMETER(nullparam);
    Player *player = static_cast<Player *>(self);
    if (player->macroIdx >= static_cast<int>(player->macroCustom->macroRuntime.size())) {
        LOG_ERROR("macroIdx >= macroRuntime.size(){}", "");
        return;
    }
    player->macroCustom->macroRuntime.at(player->macroIdx)(player);
    Event::add(getDelay(player), callbackMacroCustom, self, nullptr);
}

static void callbackNormalAttack(void *self, void *nullparam) {
    UNREFERENCED_PARAMETER(nullparam);
    Player *player = static_cast<Player *>(self);
    Event::add(player->normalAttack() + getDelayAdd(player), callbackNormalAttack, self, nullptr);
}
