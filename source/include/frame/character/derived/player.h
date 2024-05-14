#pragma once

#include "concrete.h"
#include "frame/character/character.h"
#include "frame/common/constant.h"
#include "frame/common/event.h"
#include "frame/custom.h"

namespace jx3calc {

namespace frame {
class Player;
}

namespace concrete {
template <Player type>
extern auto createPlayer() -> std::unique_ptr<frame::Player>;
auto        createPlayer(Player type) -> std::unique_ptr<frame::Player>;
} // namespace concrete

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

        // The newSkill's recipes will be used first, and the oldSkill's recipes
        // will be appended in order. Duplicates and redundancies will be discarded.
        // e.g. newSkill.recipes = {1, 2}, oldSkill.recipes = {2, 3, 4, 5}, then
        // the result will be {1, 2, 3, 4}.
        static Skill override(const Skill &oldSkill, const Skill &newSkill);
    };
    using typeSkillMap    = std::unordered_map<int, Skill>;
    using typeTalentArray = std::array<int, CountTalents>;

    // Every item in newSkills must be existed in oldSkills, otherwise it will be ignored.
    static typeSkillMap overrideSkill(const typeSkillMap &oldSkills, const typeSkillMap &newSkills);

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

    std::shared_ptr<CustomLua> customLua;

    // 战斗类型. -1 代表自定义战斗(不使用内置), 非负数代表使用内置, 且数值等于内置枚举索引, 其中 0 为默认内置循环.
    int                fightType     = 0;
    // 战斗时间. 超出该时间将停止战斗.
    event_tick_t       fightTick     = 0;
    // 等待停止标志. 当该标志 has_value() 时, 将不再以战斗时间作为判断停止的依据, 而是将该值为 0 作为停止的依据.
    std::optional<int> fightStopWait = std::nullopt;

    void init(const typeSkillMap &skills, const typeTalentArray &talents);
    void fightStart();
};
} // namespace frame

} // namespace jx3calc
