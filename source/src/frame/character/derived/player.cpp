#include "frame/character/derived/player.h"
#include "frame/event.h"
#include "frame/global/skill.h"
#include "plugin/log.h"
#include <random>

#define UNREFERENCED_PARAMETER(P) (P)

using namespace jx3calc;
using namespace frame;

Player::Player(
    int         kungfuID,
    int         kungfuLevel,
    const vtii *skills,
    const vi   *talents,
    const vi   *recipes,
    int         publicCooldownID,
    int         delayNetwork,
    int         delayKeyboard
)
    : Character(),
      initSkills(skills),
      initTalents(talents),
      initRecipes(recipes),
      publicCooldownID(publicCooldownID),
      delayBase(delayNetwork),
      delayRand(delayKeyboard) {

    this->isPlayer    = true;
    this->kungfuID    = kungfuID;
    this->kungfuLevel = kungfuLevel;

    skillLearn(kungfuID, kungfuLevel);
    skillActive(kungfuID);
}

void Player::init() {
    for (auto &[skillID, skillLevel] : *initSkills) {
        skillLearn(skillID, skillLevel);
        auto &skill = SkillManager::get(skillID, skillLevel);
        if (skill.IsPassiveSkill) {
            skillActive(skillID);
        }
    }
    for (auto &skillID : *initTalents) {
        skillLearn(skillID, 1);
        auto &skill = SkillManager::get(skillID, 1);
        if (skill.IsPassiveSkill) {
            skillActive(skillID);
        }
    }
    for (auto &recipeID : *initRecipes) {
        skillrecipeAdd(recipeID, 1);
    }
}

static void callbackMacroDefault(void *self, void *nullparam);
static void callbackMacroCustomLua(void *self, void *nullparam);
static void callbackNormalAttack(void *self, void *nullparam);

void Player::fightStart() {
    init();         // 初始化
    fightPrepare(); // 战斗准备
    if (nullptr == customLua) {
        callbackMacroDefault(this, nullptr); // 进入战斗
    } else {
        customLua->fightPrepareAdd();          // 初始化
        callbackMacroCustomLua(this, nullptr); // 进入战斗
    }
    callbackNormalAttack(this, nullptr); // 开启普通攻击
}

// 计算网络延迟和按键延迟
inline static frame::event_tick_t getDelayAdd(Player *player) {
    std::random_device              rd;
    std::mt19937                    gen(rd());
    std::uniform_int_distribution<> dis(0, player->delayRand);
    return player->delayBase + dis(gen);
}

// 计算下一次技能延迟
inline static frame::event_tick_t getDelay(Player *player) {
    frame::event_tick_t delay = 0;
    if (player->delayCustom > 0) {
        // 优先使用自定义延迟
        delay               = player->delayCustom;
        player->delayCustom = 0;
    } else if (player->publicCooldownID > 0) {
        // 其次使用 GCD
        frame::event_tick_t now  = frame::Event::now();
        frame::event_tick_t over = now;
        if (player->chCooldown.cooldownList.contains(player->publicCooldownID))
            over = player->chCooldown.cooldownList.at(player->publicCooldownID).tickOver;
        delay = over > now ? over - now : 0; // 当前无技能可放, 将延迟设为 0.
    }
    // 加上网络延迟和按键延迟
    delay += getDelayAdd(player);
    return delay;
}

static void callbackMacroDefault(void *self, void *nullparam) {
    UNREFERENCED_PARAMETER(nullparam);
    Player *player = static_cast<Player *>(self);
    player->fightDefault();
    Event::add(getDelay(player), callbackMacroDefault, self, nullptr);
}

static void callbackMacroCustomLua(void *self, void *nullparam) {
    UNREFERENCED_PARAMETER(nullparam);
    Player *player = static_cast<Player *>(self);
    if (player->macroIdx >= static_cast<int>(player->customLua->macroRuntime.size())) {
        CONSTEXPR_LOG_ERROR("macroIdx >= macroRuntime.size(){}", "");
        return;
    }
    player->customLua->macroRuntime.at(player->macroIdx)(player);
    Event::add(getDelay(player), callbackMacroCustomLua, self, nullptr);
}

static void callbackNormalAttack(void *self, void *nullparam) {
    UNREFERENCED_PARAMETER(nullparam);
    Player *player = static_cast<Player *>(self);
    Event::add(player->fightNormalAttack() + getDelayAdd(player), callbackNormalAttack, self, nullptr);
}
