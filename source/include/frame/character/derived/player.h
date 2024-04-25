#pragma once

#include "frame/character/character.h"
#include "frame/custom.h"

namespace jx3calc {
namespace frame {

class Player : public Character {
    using vtii = std::vector<std::tuple<int, int>>;
    using vi   = std::vector<int>;

public:
    Player(
        int         kungfuID,
        int         kungfuLevel,
        const vtii *skills,
        const vi   *talents,
        const vi   *recipes,
        int         publicCooldownID
    );
    virtual void fightPrepare() {}
    virtual int  fightNormalAttack() {
        return 1024; // 返回普通攻击间隔
    }
    virtual void fightDefault() {}

    int publicCooldownID = 0;
    int delayBase        = 0;
    int delayRand        = 0;
    int delayCustom      = 0;
    int macroIdx         = 0; // 当前宏索引
    int embedFightType   = 0; // 内置战斗的类型

    std::shared_ptr<CustomLua> customLua;

    const vtii *initSkills; // 目前未被 custom 启用
    const vi   *initTalents;
    const vi   *initRecipes;

    std::optional<bool> stopInitiative = std::nullopt; // 主动停止 (而非时间到而停止) 标志

    void init();
    void fightStart();
};

} // namespace frame
} // namespace jx3calc
