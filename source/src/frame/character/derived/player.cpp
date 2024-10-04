#include "frame/character/player.h"
#include "frame/event.h"
#include "frame/global/skill.h"
#include "plugin/log.h"
#include <random>

#define UNREFERENCED_PARAMETER(P) (P)

using namespace jx3calc;
using namespace frame;

Player::Player(
    int mountID,
    int kungfuID,
    int kungfuLevel,
    int publicCooldownID
) :
    Character(),
    publicCooldownID(publicCooldownID) {
    this->isPlayer    = true;
    this->mountID     = mountID;
    this->kungfuID    = kungfuID;
    this->kungfuLevel = kungfuLevel;

    skillLearn(kungfuID, kungfuLevel);
    skillActive(kungfuID);

    if (kungfuLevel == 0) { // kungfuLevel 为 0 时, 会学习该技能的最高等级.
        this->kungfuLevel = skillGetLevel(kungfuID);
    }
}

void Player::init(const typeSkills &skills, const typeTalents &talents, const typeRecipes &recipes) {
    // 技能
    for (const auto &it : skills) {
        skillLearn(it, 0);
    }
    // 奇穴
    for (const auto &it : talents) {
        skillLearn(it, 1);
        auto &skill = SkillManager::get(it, 1);
        if (skill.IsPassiveSkill) {
            skillActive(it);
        }
    }
    // 秘籍
    for (const auto &it : recipes) {
        for (const auto &recipe : it.second) {
            if (recipe > 0) {
                skillrecipeAdd(recipe, 1);
            }
        }
    }
}

static void cb_macro_default(void *self, void *nullparam);
static void cb_macro_customlua(void *self, void *nullparam);
static void cb_weapon_attack(void *self, void *nullparam);

void Player::fightStart() {
    fightPrepare(); // 战斗准备
    if (nullptr == customLua) {
        cb_macro_default(this, nullptr); // 进入战斗
    } else {
        customLua->fightPrepareAdd();      // 初始化
        cb_macro_customlua(this, nullptr); // 进入战斗
    }
    cb_weapon_attack(this, nullptr); // 开启普通攻击
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
            over = player->chCooldown.cooldownList.at(player->publicCooldownID).tickOverCurr;
        delay = over > now ? over - now : 0; // 当前无技能可放, 将延迟设为 0.
    }
    // 加上网络延迟和按键延迟
    delay += getDelayAdd(player);
    return delay;
}

static void cb_macro_default(void *self, void *nullparam) {
    UNREFERENCED_PARAMETER(nullparam);
    Player *player = static_cast<Player *>(self);
    player->fightEmbed();
    Event::add(getDelay(player), cb_macro_default, self, nullptr);
}

static void cb_macro_customlua(void *self, void *nullparam) {
    UNREFERENCED_PARAMETER(nullparam);
    Player *player = static_cast<Player *>(self);
    if (player->macroIdx >= static_cast<int>(player->customLua->macroRuntime.size())) {
        CONSTEXPR_LOG_ERROR("macroIdx >= macroRuntime.size(){}", "");
        return;
    }
    player->customLua->macroRuntime.at(player->macroIdx)(player);
    Event::add(getDelay(player), cb_macro_customlua, self, nullptr);
}

static void cb_weapon_attack(void *self, void *nullparam) {
    UNREFERENCED_PARAMETER(nullparam);
    Player *player = static_cast<Player *>(self);
    Event::add(player->fightWeaponAttack() + getDelayAdd(player), cb_weapon_attack, self, nullptr);
}
