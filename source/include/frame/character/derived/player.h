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
        int mountID,
        int kungfuID,
        int kungfuLevel,
        int publicCooldownID
    );

    using typeTalents = std::array<int, TALENT_COUNT>;
    using typeRecipe  = std::array<int, RECIPE_COUNT>;
    using typeRecipes = std::unordered_map<int, typeRecipe>;

    virtual auto fightWeaponAttack() -> event_tick_t = 0; // 应在其中实现普通攻击
    virtual void fightPrepare()                      = 0; // 应在其中实现开始战斗前的准备
    virtual void fightEmbed()                        = 0; // 应在其中实现内置战斗的逻辑
    /**
     * @brief 角色初始化的数据验证. 应在其中完成对奇穴和秘籍的数据验证 (并修改).
     * @note 奇穴秘籍数据的优先级应当是: 强制 > 传入 > 默认. (在继承类中自行实现)
     */
    virtual void initValidate(
        typeTalents &talents,
        typeRecipes &recipes
    ) = 0;
    /**
     * @brief 角色初始化. 应在其中完成技能, 奇穴和秘籍的初始化.
     * @note 传入的奇穴秘籍应当是经 initValidate 验证过的.
     */
    virtual void init(
        const typeTalents &talents,
        const typeRecipes &recipes
    ) = 0;

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

    void fightStart();
};
} // namespace frame

} // namespace jx3calc
