#pragma once

#include "frame/character/character.h"
#include "frame/common/constant.h"
#include "frame/common/event.h"
#include "frame/custom.h"

namespace jx3calc {
namespace frame {

class Player : public Character {

public:
    Player(
        int kungfuID,
        int kungfuLevel,
        int publicCooldownID
    );
    struct Skill {
        const int                                   id;
        const int                                   level;
        const std::array<int, CountRecipesPerSkill> recipes;

        static Skill override(const Skill &oldSkill, const Skill &newSkill);
    };
    using typeSkillMap    = std::unordered_map<int, Skill>;
    using typeTalentArray = std::array<int, CountTalents>;
    static typeSkillMap    overrideSkill(const typeSkillMap &oldSkills, const typeSkillMap &newSkills);
    static typeTalentArray overrideTalent(const typeTalentArray &oldTalents, const typeTalentArray &newTalents);

    virtual auto getSkills(const typeSkillMap &custom) -> typeSkillMap        = 0;
    virtual auto getTalents(const typeTalentArray &custom) -> typeTalentArray = 0;
    virtual auto fightWeaponAttack() -> event_tick_t                          = 0;
    virtual void fightPrepare()                                               = 0;
    virtual void fightEmbed()                                                 = 0;

    int publicCooldownID = 0;
    int delayBase        = 0;
    int delayRand        = 0;
    int delayCustom      = 0;
    int macroIdx         = 0; // 当前宏索引

    /**
     * @brief 内置状态
     * -1 代表自定义战斗(不使用内置), 非负数代表使用内置, 且数值等于内置枚举索引, 其中 0 为默认内置循环.
     */
    int embedStat = 0;

    std::shared_ptr<CustomLua> customLua;

    /**
     * @brief 主动停止标志
     * 当该标志 has_value() 时, 将不再以时间作为判断停止的依据, 而是将该值作为停止的依据.
     */
    std::optional<bool> stopInitiative = std::nullopt;

    void init(const typeSkillMap &skills, const typeTalentArray &talents);
    void fightStart();
};

} // namespace frame
} // namespace jx3calc
