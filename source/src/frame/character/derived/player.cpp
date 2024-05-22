#include "frame/character/derived/player.h"
#include "frame/event.h"
#include "frame/global/skill.h"
#include "plugin/log.h"
#include <random>

#define UNREFERENCED_PARAMETER(P) (P)

using namespace jx3calc;
using namespace frame;

Player::Skill Player::Skill::override(const Skill &oldSkill, const Skill &newSkill) {
    using t_arr   = std::array<int, CountRecipesPerSkill>;
    int   level   = newSkill.level > 0 ? newSkill.level : oldSkill.level;
    t_arr recipes = oldSkill.recipes;
    int   count   = 0;
    // 先写入新技能的秘籍
    for (int i = 0; i < CountRecipesPerSkill; i++) {
        if (newSkill.recipes[i] > 0) {
            recipes[i] = newSkill.recipes[i];
            count++;
        }
    }
    // 再写入旧技能的秘籍
    for (int i = 0; i < CountRecipesPerSkill && count < CountRecipesPerSkill; i++) {
        if (oldSkill.recipes[i] > 0) {
            // 检查是否已有该秘籍
            bool has = false;
            for (int j = 0; j < CountRecipesPerSkill; j++) {
                if (recipes[j] == oldSkill.recipes[i]) {
                    has = true;
                    break;
                }
            }
            if (!has) {
                recipes[count] = oldSkill.recipes[i];
                count++;
            }
        }
    }
    return {newSkill.id, level, recipes};
}

Player::typeSkillMap Player::overrideSkill(
    const typeSkillMap &oldSkills,
    const typeSkillMap &newSkills
) {
    typeSkillMap skills = oldSkills;
    for (const auto &it : newSkills) {
        if (skills.contains(it.first)) {
            auto new_ = Skill::override(skills.at(it.first), it.second);
            skills.erase(it.first);
            skills.emplace(it.first, new_);
        }
    }
    return skills;
}

Player::typeTalentArray Player::overrideTalent(
    const typeTalentArray &oldTalents,
    const typeTalentArray &newTalents
) {
    typeTalentArray talents = oldTalents;
    for (int i = 0; i < CountTalents; i++) {
        if (newTalents[i] > 0) {
            talents[i] = newTalents[i];
        }
    }
    return talents;
}

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
}

void Player::init(
    const typeSkillMap    &skills,
    const typeTalentArray &talents
) {
    for (const auto &it : skills) {
        skillLearn(it.second.id, it.second.level);
        auto &skill = SkillManager::get(it.second.id, it.second.level);
        if (skill.IsPassiveSkill) {
            skillActive(it.second.id);
        }
    }
    for (const auto &it : skills) {
        for (int i = 0; i < CountRecipesPerSkill; i++) {
            if (it.second.recipes[i] > 0) {
                skillrecipeAdd(it.second.recipes[i], 1);
            }
        }
    }
    for (const auto &id : talents) {
        skillLearn(id, 1);
        auto &skill = SkillManager::get(id, 1);
        if (skill.IsPassiveSkill) {
            skillActive(id);
        }
    }
}

static void callbackMacroDefault(void *self, void *nullparam);
static void callbackMacroCustomLua(void *self, void *nullparam);
static void callbackWeaponAttack(void *self, void *nullparam);

void Player::fightStart() {
    fightPrepare(); // 战斗准备
    if (nullptr == customLua) {
        callbackMacroDefault(this, nullptr); // 进入战斗
    } else {
        customLua->fightPrepareAdd();          // 初始化
        callbackMacroCustomLua(this, nullptr); // 进入战斗
    }
    callbackWeaponAttack(this, nullptr); // 开启普通攻击
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

static void callbackMacroDefault(void *self, void *nullparam) {
    UNREFERENCED_PARAMETER(nullparam);
    Player *player = static_cast<Player *>(self);
    player->fightEmbed();
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

static void callbackWeaponAttack(void *self, void *nullparam) {
    UNREFERENCED_PARAMETER(nullparam);
    Player *player = static_cast<Player *>(self);
    Event::add(player->fightWeaponAttack() + getDelayAdd(player), callbackWeaponAttack, self, nullptr);
}
