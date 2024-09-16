#include "concrete.h"
#include "frame/character/derived/player.h"
#include "frame/common/constant.h"
#include "frame/common/event.h"
#include "frame/event.h"
#include "frame/global/skill.h"
#include "modules/config.h"

using namespace jx3calc;

namespace {

class MjFysj : public frame::Player {
public:
    MjFysj();

private:
    virtual void fightPrepare() override;
    virtual auto fightWeaponAttack() -> frame::event_tick_t override;
    virtual void fightEmbed() override;
    virtual void initValidate(
        typeTalents &talents,
        typeRecipes &recipes
    ) override;
    virtual void init(
        const typeTalents &talents,
        const typeRecipes &recipes
    ) override;

    int framePublicCooldown = 0;

    enum class EmbedType {
        自定义        = -1,
        // 上限循环
        上限_崇光     = 0,
        // 严格循环
        严格_崇光月日 = 1 << 5,
        严格_齐光错轴,
        严格_崇光双驱三满,
        严格_崇光双驱三满压缩,
        // 简单循环
        简单_一键 = 1 << 10,
    };

    int  nFight = 0;
    bool bFight = true;
    void fight_严格(const std::vector<int> &queue);

    void fightBurst();
    void fightConv();    // conventional fight
    void fightConvVar(); // variant conventional fight
    void fightCW();

    void fight_上限_崇光69();
    void fight_简单_一键();
    void fight_简单_崇光两键();
    void fight_简单_崇光洞若();
};
} // namespace

template <>
auto concrete::createPlayer<concrete::Player::MjFysj>() -> std::unique_ptr<frame::Player> {
    return std::make_unique<MjFysj>();
}

namespace {

constexpr int 奇穴_腾焰飞芒 = 5972;
constexpr int 奇穴_净身明礼 = 18279;
constexpr int 奇穴_诛邪镇魔 = 22888;
constexpr int 奇穴_洞若观火 = 5978;
constexpr int 奇穴_无明业火 = 6717;
constexpr int 奇穴_明光恒照 = 34383;
constexpr int 奇穴_日月同辉 = 34395;
constexpr int 奇穴_靡业报劫 = 34372;
constexpr int 奇穴_用晦而明 = 17567;
constexpr int 奇穴_净体不畏 = 25166;
constexpr int 奇穴_天地诛戮 = 5979;
constexpr int 奇穴_降灵尊   = 34378;
constexpr int 奇穴_悬象著明 = 34347;
constexpr int 奇穴_崇光斩恶 = 37337;
constexpr int 奇穴_日月齐光 = 34370;

constexpr int 技能_赤日轮     = 3962;
constexpr int 技能_幽月轮     = 3959;
constexpr int 技能_烈日斩     = 3963;
constexpr int 技能_银月斩     = 3960;
constexpr int 技能_净世破魔击 = 3967;
constexpr int 技能_生死劫     = 3966;
constexpr int 技能_光明相     = 3969;
constexpr int 技能_暗尘弥散   = 3974;
constexpr int 技能_驱夜断愁   = 3979;
constexpr int 技能_诛邪镇魔   = 22890;
constexpr int 技能_悬象著明   = 34347;
constexpr int 技能_崇光斩恶   = 37335;

constexpr int 秘籍_赤日轮_会心4      = 1005;
constexpr int 秘籍_赤日轮_伤害3      = 999;
constexpr int 秘籍_赤日轮_伤害4      = 1000;
constexpr int 秘籍_赤日轮_伤害5      = 1001;
constexpr int 秘籍_烈日斩_会心4      = 1011;
constexpr int 秘籍_烈日斩_伤害4      = 1008;
constexpr int 秘籍_烈日斩_伤害5      = 1009;
constexpr int 秘籍_烈日斩_静止10     = 1013;
constexpr int 秘籍_生死劫_伤害3      = 1621;
constexpr int 秘籍_生死劫_伤害4      = 1622;
constexpr int 秘籍_生死劫_伤害5      = 1623;
constexpr int 秘籍_净世破魔击_会心5  = 1019;
constexpr int 秘籍_净世破魔击_伤害4  = 1015;
constexpr int 秘籍_净世破魔击_伤害5  = 1016;
constexpr int 秘籍_净世破魔击_20月魂 = 5206;
constexpr int 秘籍_幽月轮_会心4      = 989;
constexpr int 秘籍_幽月轮_会心5      = 990;
constexpr int 秘籍_幽月轮_伤害3      = 984;
constexpr int 秘籍_幽月轮_伤害4      = 985;
constexpr int 秘籍_银月斩_会心3      = 992;
constexpr int 秘籍_银月斩_会心4      = 993;
constexpr int 秘籍_银月斩_会心5      = 994;
constexpr int 秘籍_光明相_调息10_1   = 1029;
constexpr int 秘籍_光明相_调息10_2   = 1030;
constexpr int 秘籍_光明相_调息10_3   = 1031;
constexpr int 秘籍_驱夜断愁_会心4    = 1055;
constexpr int 秘籍_驱夜断愁_会心5    = 1056;
constexpr int 秘籍_驱夜断愁_伤害4    = 1052;
constexpr int 秘籍_驱夜断愁_伤害5    = 1053;

const std::set<int> talentAvailable {
    奇穴_腾焰飞芒,
    奇穴_净身明礼,
    奇穴_诛邪镇魔,
    奇穴_洞若观火,
    奇穴_无明业火,
    奇穴_明光恒照,
    奇穴_日月同辉,
    奇穴_靡业报劫,
    奇穴_用晦而明,
    奇穴_净体不畏,
    奇穴_天地诛戮,
    奇穴_降灵尊,
    奇穴_悬象著明,
    奇穴_崇光斩恶,
    奇穴_日月齐光,
};
const std::set<int> recipeAvailable {
    秘籍_赤日轮_会心4,
    秘籍_赤日轮_伤害3,
    秘籍_赤日轮_伤害4,
    秘籍_赤日轮_伤害5,
    秘籍_烈日斩_会心4,
    秘籍_烈日斩_伤害4,
    秘籍_烈日斩_伤害5,
    秘籍_烈日斩_静止10,
    秘籍_生死劫_伤害3,
    秘籍_生死劫_伤害4,
    秘籍_生死劫_伤害5,
    秘籍_净世破魔击_会心5,
    秘籍_净世破魔击_伤害4,
    秘籍_净世破魔击_伤害5,
    秘籍_净世破魔击_20月魂,
    秘籍_幽月轮_会心4,
    秘籍_幽月轮_会心5,
    秘籍_幽月轮_伤害3,
    秘籍_幽月轮_伤害4,
    秘籍_银月斩_会心3,
    秘籍_银月斩_会心4,
    秘籍_银月斩_会心5,
    秘籍_光明相_调息10_1,
    秘籍_光明相_调息10_2,
    秘籍_光明相_调息10_3,
    秘籍_驱夜断愁_会心4,
    秘籍_驱夜断愁_会心5,
    秘籍_驱夜断愁_伤害4,
    秘籍_驱夜断愁_伤害5,
};

// 操作别名
constexpr int 日斩 = 技能_烈日斩;
constexpr int 月斩 = 技能_银月斩;
constexpr int 日破 = 技能_净世破魔击;
constexpr int 月破 = 技能_净世破魔击;
constexpr int 日劫 = 技能_生死劫;
constexpr int 月劫 = 技能_生死劫;
constexpr int 光明 = 技能_光明相;
constexpr int 隐身 = 技能_暗尘弥散;
constexpr int 驱夜 = 技能_驱夜断愁;
constexpr int 诛邪 = 技能_诛邪镇魔;
constexpr int 月悬 = 技能_悬象著明;
constexpr int 崇光 = 技能_崇光斩恶;
// 能够用两个字表示的特殊操作
constexpr int 腰坠 = -(1 << 10);
// 无法用两个字表示的特殊操作, 使用编号
constexpr int 特00 = -(1 << 20) + 0; // 提前隐身

MjFysj::MjFysj() :
    Player(8, 10242, modules::config::isExp() ? 14 : 13, 503) {}

void MjFysj::initValidate(typeTalents &talents, typeRecipes &recipes) {
    // 奇穴
    const typeTalents  talentsDefault {奇穴_腾焰飞芒, 奇穴_净身明礼, 奇穴_诛邪镇魔, 奇穴_无明业火, 奇穴_明光恒照, 奇穴_日月同辉, 奇穴_靡业报劫, 奇穴_用晦而明, 奇穴_净体不畏, 奇穴_降灵尊, 奇穴_悬象著明, 奇穴_崇光斩恶};
    const typeTalents  talentsForcedEmbedCG {0, 0, 奇穴_诛邪镇魔, 0, 0, 奇穴_日月同辉, 奇穴_靡业报劫, 0, 奇穴_净体不畏, 奇穴_降灵尊, 奇穴_悬象著明, 奇穴_崇光斩恶};
    const typeTalents  talentsForcedEmbedQG {0, 0, 奇穴_诛邪镇魔, 0, 0, 奇穴_日月同辉, 奇穴_靡业报劫, 0, 奇穴_净体不畏, 奇穴_降灵尊, 奇穴_悬象著明, 奇穴_日月齐光};
    typeTalents        talentsTemp;
    const typeTalents *talentsForced = nullptr;
    switch (static_cast<int>(fightType)) {
    case static_cast<int>(EmbedType::上限_崇光):
        talentsForced = &talentsForcedEmbedCG;
        break;
    case static_cast<int>(EmbedType::严格_崇光月日):
        talentsForced = &talentsForcedEmbedCG;
        break;
    case static_cast<int>(EmbedType::严格_齐光错轴):
        talentsForced = &talentsForcedEmbedQG;
        break;
    case static_cast<int>(EmbedType::严格_崇光双驱三满):
        talentsForced = &talentsForcedEmbedCG;
        break;
    case static_cast<int>(EmbedType::严格_崇光双驱三满压缩):
        talentsForced = &talentsForcedEmbedCG;
        break;
    default:
        break;
    }
    for (int i = 0; i < TALENT_COUNT; i++) {
        if (talentsForced && (*talentsForced)[i] > 0) {
            talentsTemp[i] = (*talentsForced)[i]; // 使用强制奇穴
        } else if (talents[i] > 0 && talentAvailable.contains(talents[i])) {
            talentsTemp[i] = talents[i]; // 使用传入奇穴
        } else {
            talentsTemp[i] = talentsDefault[i]; // 使用默认奇穴
        }
    }
    talents = std::move(talentsTemp);

    // 秘籍
    const typeRecipes recipesDefault {
        {技能_赤日轮, {秘籍_赤日轮_伤害5, 秘籍_赤日轮_伤害4, 秘籍_赤日轮_会心4, 秘籍_赤日轮_伤害3}},
        {技能_烈日斩, {秘籍_烈日斩_静止10, 秘籍_烈日斩_伤害5, 秘籍_烈日斩_伤害4, 秘籍_烈日斩_会心4}},
        {技能_生死劫, {秘籍_生死劫_伤害5, 秘籍_生死劫_伤害4, 秘籍_生死劫_伤害3}},
        {技能_净世破魔击, {秘籍_净世破魔击_20月魂, 秘籍_净世破魔击_会心5, 秘籍_净世破魔击_伤害5, 秘籍_净世破魔击_伤害4}},
        {技能_幽月轮, {秘籍_幽月轮_会心5, 秘籍_幽月轮_伤害4, 秘籍_幽月轮_会心4, 秘籍_幽月轮_伤害3}},
        {技能_银月斩, {秘籍_银月斩_会心5, 秘籍_银月斩_会心4, 秘籍_银月斩_会心3}},
        {技能_光明相, {秘籍_光明相_调息10_1, 秘籍_光明相_调息10_2, 秘籍_光明相_调息10_3}},
        {技能_驱夜断愁, {秘籍_驱夜断愁_伤害5, 秘籍_驱夜断愁_会心5, 秘籍_驱夜断愁_伤害4, 秘籍_驱夜断愁_会心4}},
    };
    const typeRecipes recipesForced {
        {技能_净世破魔击, {秘籍_净世破魔击_20月魂}},
    };
    typeRecipes recipesTemp;
    auto        addRecipe = [](typeRecipes &recipes, int skill, int recipe) {
        if (!recipeAvailable.contains(recipe))
            return;
        for (int i = 0; i < RECIPE_COUNT; i++) {
            if (recipes[skill][i] == 0) { // 最靠前的空位
                recipes[skill][i] = recipe;
                break;
            } else if (recipes[skill][i] == recipe) { // 已存在
                break;
            }
        }
    };
    auto addRecipes = [&addRecipe](typeRecipes &recipesTarget, const typeRecipes &recipesAdd) {
        for (const auto [skill, recipes] : recipesAdd) {
            for (const auto &recipe : recipes) {
                addRecipe(recipesTarget, skill, recipe);
            }
        }
    };
    addRecipes(recipesTemp, recipesForced);
    addRecipes(recipesTemp, recipes);
    addRecipes(recipesTemp, recipesDefault);
    recipes = std::move(recipesTemp);
}

void MjFysj::init(const typeTalents &talents, const typeRecipes &recipes) {
    // 技能
    const std::unordered_map<int, int> skills {
        {技能_赤日轮, 33},
        {技能_烈日斩, 32},
        {技能_生死劫, 1},
        {技能_净世破魔击, 32},
        {技能_幽月轮, 24},
        {技能_银月斩, 18},
        {技能_光明相, 1},
        {技能_暗尘弥散, 1},
        {技能_驱夜断愁, 29},
        {技能_诛邪镇魔, 1},
    };
    for (const auto &it : skills) {
        skillLearn(it.first, it.second);
        auto &skill = frame::SkillManager::get(it.first, it.second);
        if (skill.IsPassiveSkill) {
            skillActive(it.first);
        }
    }
    // 奇穴
    for (const auto &it : talents) {
        skillLearn(it, 1);
        auto &skill = frame::SkillManager::get(it, 1);
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

void MjFysj::fightPrepare() {
    cast(3974);
    if (nSunPowerValue == 0 && nMoonPowerValue == 0) {
        if (nCurrentMoonEnergy >= 10000)
            nMoonPowerValue = 1;
        else if (nCurrentSunEnergy >= 10000)
            nSunPowerValue = 1;
    }
    framePublicCooldown = 16 * 1024 / (1024 + chAttr.getHaste());
}

auto MjFysj::fightWeaponAttack() -> frame::event_tick_t {
    skillCast(targetSelect, 4326, 1); // 大漠刀法
    int frame = 16 * 1024 / (1024 + this->chAttr.getHaste());
    return frame * 64; // 64 = 1024/16
}

extern const std::vector<int> queue崇光月日;
extern const std::vector<int> queue齐光错劫;
extern const std::vector<int> queue崇光双驱三满;
extern const std::vector<int> queue崇光双驱三满压缩;
extern const std::vector<int> queue一键起手;

void MjFysj::fightEmbed() {
    switch (static_cast<int>(fightType)) {
    case static_cast<int>(EmbedType::上限_崇光):
        return fight_上限_崇光69();
    case static_cast<int>(EmbedType::严格_崇光月日):         return fight_严格(queue崇光月日);
    case static_cast<int>(EmbedType::严格_齐光错轴):         return fight_严格(queue齐光错劫);
    case static_cast<int>(EmbedType::严格_崇光双驱三满):     return fight_严格(queue崇光双驱三满);
    case static_cast<int>(EmbedType::严格_崇光双驱三满压缩): return fight_严格(queue崇光双驱三满压缩);
    default:                                                 return fight_简单_一键();
    }
}

constexpr int delay隐身 = 50; // 延迟释放隐身时间: 50ms

void MjFysj::fight_严格(const std::vector<int> &queue) {
    if (nFight == 0) [[unlikely]]
        fightStopWait.emplace(1);
    if (nFight >= queue.size()) [[unlikely]]
        fightStopWait.emplace(0);
    while (queue[nFight] < 0) {
        switch (queue[nFight]) {
        case 腰坠: {
            itemUse(frame::ItemType::Trinket, 39853); // 梧桐影
            itemUse(frame::ItemType::Trinket, 38789); // 吹香雪
        } break;
        case 特00: {
            const auto cd = skillCooldownLeftTick(技能_暗尘弥散);
            delayCustom   = static_cast<int>(cd < delay隐身 ? delay隐身 : cd) + delayBase + delayRand / 2;
        } break;
        }
        nFight++;
    }
    const auto &skill = queue[nFight];
    if (cast(skill)) {
        nFight++;
    } else {
        const auto tick = skillCooldownLeftTick(skill);
        if (tick > 0)
            return; // 未冷却完毕, 返回等待冷却
        else
            fightStopWait.reset(); // 冷却完毕, 但无法释放, 结束战斗
    }
    if (nFight == queue.size()) [[unlikely]]
        fightStopWait.emplace(0);
}

void MjFysj::fight_简单_一键() {
    if (bFight) {
        cast(queue一键起手[nFight]);
        nFight++;
        if (nFight == queue一键起手.size()) {
            bFight = false;
        }
    } else [[likely]] {
        const auto buff崇光 = buffGet(28194, 1);
        const auto buff斩恶 = buffGet(28195, 1);
        const auto buff连击 = buffGet(28196, 1);
        if (buff连击 && buff连击->isValid && buff连击->nStackNum < 3)
            cast(37335); // 崇光斩恶, 连击
        if (buff崇光 && buff崇光->isValid && buff崇光->nStackNum == 6 && buff斩恶 && buff斩恶->isValid)
            cast(37335); // 崇光斩恶, 5.5 层
        if (nCurrentMoonEnergy <= 2000)
            cast(技能_暗尘弥散);
        const auto buff降灵尊 = buffGet(25731, 1);
        if (buff降灵尊 && buff降灵尊->isValid) {
            itemUse(frame::ItemType::Trinket, 38789); // 吹香雪
            itemUse(frame::ItemType::Trinket, 39853); // 梧桐影
            cast(技能_悬象著明);
        }
        cast(技能_生死劫);
        cast(技能_诛邪镇魔);
        cast(技能_净世破魔击);
        if (!(buff降灵尊 && buff降灵尊->isValid) && buff崇光 && buff崇光->isValid && buff崇光->nStackNum >= 3)
            cast(37335); // 崇光斩恶, 3 层
        if (nCurrentMoonEnergy <= 6000)
            cast(技能_驱夜断愁);
        cast(技能_烈日斩);
        cast(技能_银月斩);
        cast(技能_幽月轮);
    }
}

const std::vector<int> queue一键起手 {
    日斩, 月斩
};

const std::vector<int> queue崇光月日 {
    日斩, 月斩, 月悬, 特00, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 崇光, 崇光, 崇光, 诛邪, 月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 崇光, 崇光, 崇光, 诛邪, 月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    日斩, 月斩, 驱夜, 月悬, 特00, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 腰坠, 日破, 诛邪, 月破, 诛邪,
    月斩, 日斩, 崇光, 崇光, 崇光, 驱夜, 月破, 日劫, 崇光, 崇光, 崇光, 诛邪, 月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 诛邪, 月斩, 日斩, 驱夜, 月破, 日破, 诛邪, 崇光, 崇光, 崇光,
    日斩, 月斩, 驱夜, 月悬, 特00, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 崇光, 崇光, 崇光, 诛邪, 月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 崇光, 崇光, 崇光, 诛邪, 月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    日斩, 月斩, 驱夜, 月悬, 特00, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 腰坠, 日破, 诛邪, 月破, 诛邪,
    月斩, 日斩, 崇光, 崇光, 崇光, 驱夜, 月破, 日劫, 崇光, 崇光, 崇光, 诛邪, 月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 诛邪, 月斩, 日斩, 驱夜, 月破, 日破, 诛邪, 崇光, 崇光, 崇光,
    日斩, 月斩, 驱夜, 月悬, 特00, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 崇光, 崇光, 崇光, 诛邪, 月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 崇光, 崇光, 崇光, 诛邪, 月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    日斩, 月斩, 驱夜, 月悬, 特00, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 腰坠, 日破, 诛邪, 月破, 诛邪,
    月斩, 日斩, 崇光, 崇光, 崇光, 驱夜, 月破, 日劫, 崇光, 崇光, 崇光, 诛邪, 月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 诛邪, 月斩, 日斩, 驱夜, 月破, 日破, 诛邪, 崇光, 崇光, 崇光,
    日斩, 月斩, 驱夜, 月悬, 特00, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 腰坠, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪, 日斩, 崇光, 崇光, 崇光
};

const std::vector<int> queue崇光双驱三满 {
    日斩, 月斩, 月悬, 特00, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪, // 降灵尊
    日斩, 驱夜, 日破, 月斩, 月劫, 诛邪,                                     // 2斩2满
    日斩, 驱夜, 日破, 崇光, 崇光, 崇光,                                     // 1斩1满 + 3崇
    日斩, 驱夜, 月破, 诛邪, 日破,                                           // [1斩2满]
    月斩, 日斩, 驱夜, 月劫, 诛邪, 日破,                                     // 2斩2满, 打劫
    月斩, 日斩, 驱夜, 月破, 诛邪, 日破, 崇光, 崇光, 崇光,                   // 2斩2满 + 3崇
    日斩, 月斩, 驱夜, 月悬, 日劫, 特00, 诛邪, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 腰坠, 月破, 诛邪, // 降灵尊
    日斩, 驱夜, 日破, 月斩, 崇光, 崇光, 崇光, 月劫, 诛邪, 日斩, 驱夜, 日破, 崇光, 崇光, 崇光,
    日斩, 驱夜, 月破, 诛邪, 日破, 月斩, 日斩, 驱夜, 月劫, 诛邪, 日破,
    月斩, 日斩, 驱夜, 月破, 诛邪, 日破, 崇光, 崇光, 崇光,
    日斩, 月斩, 驱夜, 月悬, 特00, 日劫, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 腰坠, 月破, 诛邪, // 降灵尊
    日斩, 驱夜, 日破, 月斩, 崇光, 崇光, 崇光, 月劫, 诛邪, 日斩, 驱夜, 日破, 崇光, 崇光, 崇光,
    日斩, 驱夜, 月破, 诛邪, 日破, 月斩, 日斩, 驱夜, 月劫, 诛邪, 日破,
    月斩, 日斩, 驱夜, 月破, 诛邪, 日破, 崇光, 崇光, 崇光,
    日斩, 月斩, 驱夜, 月悬, 特00, 日劫, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪, // 降灵尊
    日斩, 驱夜, 日破, 月斩, 月劫, 诛邪, 日斩, 驱夜, 日破, 崇光, 崇光, 崇光,
    日斩, 驱夜, 月破, 诛邪, 日破, 月斩, 日斩, 驱夜, 月劫, 诛邪, 日破,
    月斩, 日斩, 驱夜, 月破, 诛邪, 日破, 崇光, 崇光, 崇光,
    日斩, 月斩, 驱夜, 月悬, 特00, 日劫, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 腰坠, 月破, 诛邪, // 降灵尊
    日斩, 驱夜, 日破, 月斩, 崇光, 崇光, 崇光, 月劫, 诛邪, 日斩, 驱夜, 日破, 崇光, 崇光, 崇光,
    日斩, 驱夜, 月破, 诛邪, 日破, 月斩, 日斩, 驱夜, 月劫, 诛邪, 日破,
    月斩, 日斩, 驱夜, 月破, 诛邪, 日破, 崇光, 崇光, 崇光,
    日斩, 月斩, 驱夜, 月悬, 特00, 日劫, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 腰坠, 月破, 诛邪, // 降灵尊
    日斩, 驱夜, 日破, 月斩, 崇光, 崇光, 崇光, 月劫, 诛邪, 日斩, 驱夜, 日破, 崇光, 崇光, 崇光,
    日斩, 驱夜, 月破, 诛邪, 日破, 月斩, 日斩, 驱夜, 月劫, 诛邪, 日破,
    月斩, 日斩, 驱夜, 月破, 诛邪, 日破, 崇光, 崇光, 崇光,
    日斩, 月斩, 驱夜, 月悬, 特00, 日劫, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪, 日斩, 崇光, 崇光, 崇光
};

const std::vector<int> queue崇光双驱三满压缩 {
    日斩, 月斩, 月悬, 特00, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪, // 降灵尊
    日斩, 驱夜, 日劫, 月斩, 月破,                                           // 2斩2满
    日斩, 驱夜, 日破, 诛邪, 崇光, 崇光, 崇光,                               // 1斩1满 + 3崇
    日斩, 驱夜, 月破, 日破, 诛邪,                                           // [1斩2满]
    日斩, 月斩, 驱夜, 月劫, 日破,                                           // 2斩2满, 打劫
    日斩, 月斩, 驱夜, 月破, 诛邪, 崇光, 崇光, 崇光, 日破,                   // 2斩2满 + 3崇
    日斩, 月斩, 驱夜, 月悬, 特00, 日劫, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, // 降灵尊
    日斩, 驱夜, 腰坠, 诛邪, 日劫, 月斩, 崇光, 崇光, 崇光, 月破,       // 2斩2满
    日斩, 驱夜, 日破, 诛邪, 崇光, 崇光, 崇光,                         // 1斩1满 + 3崇
    日斩, 驱夜, 月破, 日劫, 诛邪,                                     // [1斩2满], 打劫
    日斩, 月斩, 驱夜, 月破, 日破, 诛邪, 崇光, 崇光, 崇光,             // 2斩2满 + 3崇
    日斩, 月斩, 驱夜, 月悬, 特00, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    日斩, 驱夜, 日劫, 月斩, 月破, 日斩, 驱夜, 日破, 诛邪, 崇光, 崇光, 崇光, 日斩, 驱夜, 月破, 日破, 诛邪, // [1斩2满] 收尾
    日斩, 月斩, 驱夜, 月劫, 日破, 日斩, 月斩, 驱夜, 月破, 诛邪, 崇光, 崇光, 崇光, 日破,
    日斩, 月斩, 驱夜, 月悬, 特00, 日劫, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破,
    日斩, 驱夜, 腰坠, 诛邪, 日劫, 月斩, 崇光, 崇光, 崇光, 月破, 日斩, 驱夜, 日破, 诛邪, 崇光, 崇光, 崇光, 日斩, 驱夜, 月破, 日劫, 诛邪, // [1斩2满] 收尾
    日斩, 月斩, 驱夜, 月破, 日破, 诛邪, 崇光, 崇光, 崇光,
    日斩, 月斩, 驱夜, 月悬, 特00, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    日斩, 驱夜, 日劫, 月斩, 月破, 日斩, 驱夜, 日破, 诛邪, 崇光, 崇光, 崇光, 日斩, 驱夜, 月破, 日破, 诛邪, // [1斩2满] 收尾
    日斩, 月斩, 驱夜, 月劫, 日破, 日斩, 月斩, 驱夜, 月破, 诛邪, 崇光, 崇光, 崇光, 日破,
    日斩, 月斩, 驱夜, 月悬, 特00, 日劫, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破,
    日斩, 驱夜, 腰坠, 诛邪, 日劫, 月斩, 崇光, 崇光, 崇光, 月破, 日斩, 驱夜, 日破, 诛邪, 崇光, 崇光, 崇光, 日斩, 驱夜, 月破, 日劫, 诛邪, // [1斩2满] 收尾
    日斩, 月斩, 驱夜, 月破, 日破, 诛邪, 崇光, 崇光, 崇光,
    日斩, 月斩, 驱夜, 月悬, 特00, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪, 日斩, 崇光, 崇光, 崇光
};

const std::vector<int> queue齐光错劫 {
    日斩, 月斩, 月悬, 腰坠, 光明, 特00, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月悬, 特00, 日破, 隐身, 诛邪,
    月破, 日劫, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 诛邪,
    月斩, 日斩, 驱夜, 月悬, 特00, 日破, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 光明, 日劫, 月破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月悬, 特00, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月悬, 特00, 日破, 隐身, 光明, 诛邪,
    月破, 日劫, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 诛邪,
    月斩, 日斩, 驱夜, 月悬, 特00, 日破, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日劫, 月破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日破, 诛邪,
    月斩, 日斩, 驱夜, 月悬, 腰坠, 光明, 特00, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪
};

void MjFysj::fightBurst() {
    // 进入此小节, 隐身冷却时间一定小于 3 个 GCD.
    // 悬象一定冷却完毕, 因为上一次悬象的释放时机一定在: 隐身前 3 个 GCD ~ 隐身后瞬间.

    // 获取计算资源
    const auto buff悬象   = buffGet(25716, 0);
    const auto buff橙武CD = buffGet(2584, 0);
    const auto buff灵日   = buffGet(9910, 0);
    const auto buff魂月   = buffGet(9911, 0);
    const auto cd暗尘弥散 = skillCooldownLeftTick(技能_暗尘弥散);
    // 注意: 以下计算资源为当前一次性计算, 并不会随计算源而更新, 使用时需注意
    const auto time半边诛邪 =
        buff灵日 && buff灵日->isValid
            ? buff灵日->nLeftFrame * 1024 / 16
        : buff魂月 && buff魂月->isValid
            ? buff魂月->nLeftFrame * 1024 / 16
            : 0;
    const auto time橙武CD  = buff橙武CD && buff橙武CD->isValid ? buff橙武CD->nLeftFrame * 1024 / 16 : 0;
    const auto time最长GCD = framePublicCooldown * 1024 / 16 + delayBase + delayRand;

    // 施展隐身. 可能在GCD内释放, 也可能在释放其他技能前释放.
    if ((nCurrentSunEnergy < 10000 && nCurrentMoonEnergy < 10000) || // 日灵月魂均小于100, 或
        time橙武CD > 26 * 1024 + delayBase + delayRand)              // 在橙武特效中, 且不在最后一秒
        if (cast(技能_暗尘弥散))
            return;

    // 直接施展悬象
    if (cast(技能_悬象著明))
        return;
    // 处理一种特殊情况: 没有半边诛邪, 且隐身 CD 大于 1 GCD 小于 3 GCD, 打破魔击
    if (time半边诛邪 == 0 && cd暗尘弥散 > time最长GCD && cd暗尘弥散 < 3 * time最长GCD)
        if (cast(技能_净世破魔击))
            return;
    // 施展悬象反色生死劫
    if (buff悬象 && buff悬象->isValid)
        if ((nMoonPowerValue && buff悬象->nLevel == 1) || // 满月且有日悬, 或者
            (nSunPowerValue && buff悬象->nLevel == 2)) {  // 满日且有月悬
            if (cast(技能_生死劫)) {
                if (cd暗尘弥散 < time最长GCD)
                    delayCustom = static_cast<int>(cd暗尘弥散 < delay隐身 ? delay隐身 : cd暗尘弥散) + delayBase + delayRand / 2;
                return;
                // } else if (cd生死劫 < 400) {
                //     delayCustom = static_cast<int>(cd生死劫 + delayBase + delayRand / 2);
                //     return;
            }
        }
    // 满灵时施展诛邪
    if (nSunPowerValue || nMoonPowerValue)
        if (cast(技能_诛邪镇魔))
            return;
    // 施展破魔击
    if (cast(技能_净世破魔击)) {
        if (cd暗尘弥散 < time最长GCD)
            delayCustom = static_cast<int>(cd暗尘弥散 < delay隐身 ? delay隐身 : cd暗尘弥散) + delayBase + delayRand / 2;
        return;
    }
    // 处理一种特殊情况: 还有3层悬象, 而灵魂状态是 x/0.
    // 此时如果直接打驱夜, 会出现问题:
    // (x/0)-驱夜(100/60)-诛邪-日破(0/80,2)-诛邪-驱夜(60/100)-月破(80/20,1)-诛邪(卡住)
    // 在橙武特效中可能出现这样的问题. 例如: 日悬-月破-日破-月劫(x/0) 接上述情况.
    // 此处对其进行特殊处理, 在这种情况下打一个月斩.
    if (buff悬象 && buff悬象->isValid && buff悬象->nStackNum == 3 && nCurrentSunEnergy >= 40 && nCurrentMoonEnergy == 0)
        if (cast(技能_银月斩))
            return;
    // 施展驱夜断愁
    if (cast(技能_驱夜断愁))
        return;
    // 施展诛邪
    if (cast(技能_诛邪镇魔))
        return;
    // 可能的日月斩 (由橙武特效引起)
    if (nCurrentSunEnergy + nCurrentMoonEnergy >= 14000)
        if (cast(nCurrentMoonEnergy > nCurrentSunEnergy ? 技能_烈日斩 : 技能_银月斩))
            return;
}

void MjFysj::fightConv() {
    // 获取计算资源
    const auto buff目标月斩 = targetSelect ? targetSelect->buffGet(4202, 0) : nullptr;
    const auto buff诛邪     = buffGet(9909, 0);
    const auto buff非侠     = buffGet(28886, 1);
    const auto buff降灵尊   = buffGet(25731, 1);
    const auto cd驱夜断愁   = skillCooldownLeftTick(技能_驱夜断愁);
    const auto cd生死劫     = skillCooldownLeftTick(技能_生死劫);
    // 注意: 以下计算资源为当前一次性计算, 并不会随计算源而更新, 使用时需注意
    const auto time目标月斩 = buff目标月斩 && buff目标月斩->isValid ? buff目标月斩->nLeftFrame * 1024 / 16 : 0;
    const auto time诛邪     = buff诛邪 && buff诛邪->isValid ? buff诛邪->nLeftFrame * 1024 / 16 : 0;
    const auto time非侠     = buff非侠 && buff非侠->isValid ? buff非侠->nLeftFrame * 1024 / 16 : 0;
    const auto time最短GCD  = framePublicCooldown * 1024 / 16;
    const auto time最长GCD  = framePublicCooldown * 1024 / 16 + delayBase + delayRand;

    // 诛邪
    if (time非侠 < time最短GCD ||                  // 非侠 buff 不存在或即将消失, 或
        time诛邪 < 1024 + delayBase + delayRand || // 最后一秒, 或者
        nSunPowerValue || nMoonPowerValue ||       // 满灵, 或者
        (buff降灵尊 && buff降灵尊->isValid)        // 降灵尊内
    )
        if (cast(技能_诛邪镇魔))
            return;
    // 处理一种特殊情况: 双满灵时, 非侠 buff 剩余时间小于 5 个 GCD, 手头没有半边诛邪
    // × 劫 破 斩 斩 驱 (非侠消失) 破 诛
    if (time非侠 < 5 * time最长GCD && cd生死劫 == 0 &&
        nCurrentSunEnergy >= 10000 && nCurrentMoonEnergy >= 10000 &&
        !buffExist(9910, 0) && !buffExist(9911, 0))
        if (cast(技能_净世破魔击))
            return;
    // 生死劫, 无条件施展
    if (cast(技能_生死劫))
        return;
    // 破魔击, 无条件施展
    if (cast(技能_净世破魔击))
        return;
    // 驱夜
    if (nCurrentSunEnergy >= 4000)
        if (cast(技能_驱夜断愁))
            return;
    // 日斩
    if (buffExist(25759, 0) ||                                                       // 有日增伤明光, 或
        (nCurrentMoonEnergy >= 4000 && cd驱夜断愁 < time最短GCD && time目标月斩 > 0) // 月 >= 40, 且驱夜断愁冷却时间小于 1 秒, 且至少有月斩 buff
    )
        if (cast(技能_烈日斩))
            return;
    // 月斩
    const auto timeCmp = // 9 跳月斩时间 - 6 GCD 时间
        ((framePublicCooldown * 2 + 1) * 9 - 6 * framePublicCooldown) * 1024 / 16;
    if (nCurrentMoonEnergy <= 2000 ||                          // 月 <= 20, 或
        (nCurrentMoonEnergy >= 6000 && time目标月斩 < timeCmp) // 月 >= 60但上一个技能不是月斩
    )
        if (cast(技能_银月斩))
            return;
    // 日斩
    if (cast(技能_烈日斩))
        return;
}

void MjFysj::fightCW() {
    // 获取计算资源
    const auto cd生死劫   = skillCooldownLeftTick(技能_生死劫);
    const auto cd暗尘弥散 = skillCooldownLeftTick(技能_暗尘弥散);
    const auto buff悬象   = buffGet(25716, 0);
    const auto buff橙武CD = buffGet(2584, 0);
    const auto buff非侠   = buffGet(28886, 1);
    // 注意: 以下计算资源为当前一次性计算, 并不会随计算源而更新, 使用时需注意
    const auto time橙武CD = buff橙武CD && buff橙武CD->isValid ? buff橙武CD->nLeftFrame * 1024 / 16 : 0;
    const auto time非侠   = buff非侠 && buff非侠->isValid ? buff非侠->nLeftFrame * 1024 / 16 : 0;

    // 开特效腰坠
    itemUse(frame::ItemType::Trinket, 39853); // 梧桐影
    itemUse(frame::ItemType::Trinket, 38789); // 吹香雪

    // 悬象. 可以提前放.
    if (time橙武CD > 27 * 1024 && cd暗尘弥散 < 1024)
        cast(技能_悬象著明); // 提前放悬象
    // 隐身
    if (time橙武CD > 26 * 1024 && cd暗尘弥散 == 0) {
        cast(技能_暗尘弥散); // 一定能放出来
        cast(技能_悬象著明); // 不一定放得出来, 因为有可能悬象提前放了
    }
    // 诛邪
    if (time非侠 == 0)
        cast(技能_诛邪镇魔);
    // 生死劫
    if (buff悬象 && buff悬象->isValid) {                  // 如果有悬象, 那么必须满足以下二者之一, 才施展:
        if ((nMoonPowerValue && buff悬象->nLevel == 1) || // 1. 满月且有日悬, 或者
            (nSunPowerValue && buff悬象->nLevel == 2)) {  // 2. 满日且有月悬
            // cast(生死劫);
            if (cast(技能_生死劫))
                return;
            else if (cd生死劫 < 400) {
                delayCustom = static_cast<int>(cd生死劫 + delayBase + delayRand / 2);
                return;
            }
        }
    } else { // 否则(如果没有悬象), 可以直接施展
        cast(技能_生死劫);
    }
    cast(技能_净世破魔击);
}

void MjFysj::fight_上限_崇光69() {
    // 获取计算资源
    const auto currentTick  = frame::Event::now();
    const auto buff目标日斩 = targetSelect ? targetSelect->buffGet(4418, 1) : nullptr;
    const auto buff目标月斩 = targetSelect ? targetSelect->buffGet(4202, 0) : nullptr;
    const auto cd生死劫     = skillCooldownLeftTick(技能_生死劫);
    const auto cd暗尘弥散   = skillCooldownLeftTick(技能_暗尘弥散);
    const auto cd驱夜断愁   = skillCooldownLeftTick(技能_驱夜断愁);
    const auto buff悬象     = buffGet(25716, 0);
    const auto buff崇光     = buffGet(28194, 1);
    const auto buff斩恶     = buffGet(28195, 1);
    const auto buff连击     = buffGet(28196, 1);
    const auto buff橙武CD   = buffGet(2584, 0);
    // 注意: 以下计算资源为当前一次性计算, 并不会随计算源而更新, 使用时需注意
    const auto time目标日斩 = buff目标日斩 && buff目标日斩->isValid ? buff目标日斩->nLeftFrame * 1024 / 16 : 0;
    const auto time目标月斩 = buff目标月斩 && buff目标月斩->isValid ? buff目标月斩->nLeftFrame * 1024 / 16 : 0;
    const auto stacknum崇光 = buff崇光 && buff崇光->isValid ? buff崇光->nStackNum : 0;
    const auto stacknum连击 = buff连击 && buff连击->isValid ? buff连击->nStackNum : 0;
    const auto time橙武CD   = buff橙武CD && buff橙武CD->isValid ? buff橙武CD->nLeftFrame * 1024 / 16 : 0;
    const auto time最长GCD  = framePublicCooldown * 1024 / 16 + delayBase + delayRand;

    enum 战斗状态 {
        停止,
        正常,
        收尾
    };

    // 起手双斩
    if (currentTick < 3 * 1024) {
        fightStopWait.emplace(战斗状态::正常);
        if (time目标日斩 == 0)
            if (cast(技能_烈日斩))
                return;
        if (time目标月斩 == 0)
            if (cast(技能_银月斩))
                return;
    }

    // 超时结束
    if (currentTick > 2 * fightTick) {
        fightStopWait.emplace(战斗状态::停止);
        return;
    }

    // 连击崇光
    if (stacknum连击 == 2 && fightStopWait.value() == 2) {
        if (cast(技能_崇光斩恶)) {
            fightStopWait.emplace(战斗状态::停止);
            return;
        }
    } else if (stacknum连击 > 0 && stacknum连击 < 3) {
        if (cast(技能_崇光斩恶))
            return;
    }

    // 溢出崇光
    if (stacknum崇光 >= 6 && buff斩恶 && buff斩恶->isValid) {
        // 日斩
        if (time目标日斩 < 3 * time最长GCD) {
            if (cast(技能_烈日斩))
                return;
        }
        // // 月斩
        // if (time目标月斩 < 3 * time最长GCD &&
        //     time非侠 > 4 * time最长GCD &&       // 确保不会因插入月斩而导致非侠时间不足
        //     !nMoonPowerValue && !nSunPowerValue // 满灵不斩
        // ) {
        //     if (cast(银月斩))
        //         return;
        // }
        // 开特效腰坠
        itemUse(frame::ItemType::Trinket, 39853); // 梧桐影
        itemUse(frame::ItemType::Trinket, 38789); // 吹香雪
        // 崇光
        if (cast(技能_崇光斩恶))
            return;
    }

    // 橙武特效
    if (time橙武CD > 25 * 1024)
        fightCW();

    if (buff悬象 && buff悬象->isValid)
        return fightBurst();
    if (stacknum崇光 >= 3) { // 崇光
        // 降前崇
        // const auto bool诛GCD = (buff诛邪 && buff诛邪->isValid) || (buff灵日 && buff灵日->isValid) || (buff魂月 && buff魂月->isValid);
        if (cd暗尘弥散 < time最长GCD * 9 &&                         // 诛崇崇崇斩斩驱破诛
            nCurrentSunEnergy <= 2000 && nCurrentMoonEnergy <= 2000 // 空灵
        ) {
            if (cast(技能_诛邪镇魔))
                return;
            if (cast(技能_崇光斩恶))
                return;
        }
        // 0日高月, 且驱夜在 CD 且 目标存在月斩, 崇
        if (nCurrentSunEnergy == 0 && nCurrentMoonEnergy >= 6000 && !nMoonPowerValue &&
            cd驱夜断愁 > time最长GCD && time目标月斩 > 6 * 1024 // 经过 12 秒降灵尊, 月斩一定小于 6 秒
        ) {
            if (stacknum崇光 >= 5) {
                // 开特效腰坠
                itemUse(frame::ItemType::Trinket, 39853); // 梧桐影
                itemUse(frame::ItemType::Trinket, 38789); // 吹香雪
            }
            if (cast(技能_诛邪镇魔))
                return;
            if (cast(技能_崇光斩恶))
                return;
        }
        // 劫前满灵崇. 长时间无劫, 必有非侠
        if (cd生死劫 > 0 && cd生死劫 < 3 * time最长GCD &&
            (nSunPowerValue || nMoonPowerValue)) {
            if (stacknum崇光 >= 5) {
                // 开特效腰坠
                itemUse(frame::ItemType::Trinket, 39853); // 梧桐影
                itemUse(frame::ItemType::Trinket, 38789); // 吹香雪
            }
            if (cast(技能_崇光斩恶))
                return;
        }
        // 收尾日斩崇
        if (stacknum崇光 < 5 &&                  // 崇光层数小于5 (如果有5层, 就再打一个6GCD再打三崇)
            time目标日斩 > 3 * time最长GCD &&    // 目标日斩buff时长大于3秒, 并且
            currentTick - fightTick > -20 * 1024 // 距离预定的收尾时间不到20s, 或已经超过了预定的收尾时间
        )
            if (cast(技能_崇光斩恶)) {
                fightStopWait.emplace(战斗状态::收尾);
                return;
            }
    }
    if (cd暗尘弥散 < 3 * time最长GCD && nCurrentSunEnergy >= 10000 && nCurrentMoonEnergy >= 10000)
        // 隐身 CD 小于 3 GCD 且双满, 进入爆发
        return fightBurst();
    return fightConv();
}

} // namespace
