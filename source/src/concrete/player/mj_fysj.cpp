#include "concrete/player.h"
#include "frame/common/event.h"
#include <memory>

using namespace jx3calc;
using namespace concrete;

namespace {

enum talent {
    腾焰飞芒_奇穴 = 5972,
    净身明礼_奇穴 = 18279,
    诛邪镇魔_奇穴 = 22888,
    无明业火_奇穴 = 6717,
    明光恒照_奇穴 = 34383,
    日月同辉_奇穴 = 34395,
    靡业报劫_奇穴 = 34372,
    用晦而明_奇穴 = 17567,
    净体不畏_奇穴 = 25166,
    降灵尊_奇穴   = 34378,
    悬象著明_奇穴 = 34347,
    崇光斩恶_奇穴 = 37337,
    日月齐光_奇穴 = 34370,
};

enum skill {
    赤日轮     = 3962,
    幽月轮     = 3959,
    烈日斩     = 3963,
    银月斩     = 3960,
    净世破魔击 = 3967,
    生死劫     = 3966,
    光明相     = 3969,
    暗尘弥散   = 3974,
    驱夜断愁   = 3979,
    诛邪镇魔   = 22890,
    悬象著明   = 34347,
    崇光斩恶   = 37335,
};
enum recipe {
    赤日轮_会心4      = 1005, // 赤日轮, 会心提高4%
    赤日轮_伤害3      = 999,  // 赤日轮, 伤害提高3%
    赤日轮_伤害4      = 1000, // 赤日轮, 伤害提高4%
    赤日轮_伤害5      = 1001, // 赤日轮, 伤害提高5%
    烈日斩_会心4      = 1011, // 烈日斩, 会心提高4%
    烈日斩_伤害4      = 1008, // 烈日斩, 伤害提高4%
    烈日斩_伤害5      = 1009, // 烈日斩, 伤害提高5%
    烈日斩_静止10     = 1013, // 烈日斩, 对原地静止的目标伤害提升10%
    生死劫_伤害3      = 1621, // 生死劫, 伤害提高3%
    生死劫_伤害4      = 1622, // 生死劫, 伤害提高4%
    生死劫_伤害5      = 1623, // 生死劫, 伤害提高5%
    净世破魔击_会心5  = 1019, // 净世破魔击, 会心提高5%
    净世破魔击_伤害4  = 1015, // 净世破魔击, 伤害提高4%
    净世破魔击_伤害5  = 1016, // 净世破魔击, 伤害提高5%
    净世破魔击_20月魂 = 5206, // 焚影圣诀心法下净世破魔击·月命中后回复20点月魂
    幽月轮_会心4      = 989,  // 幽月轮, 会心提高4%
    幽月轮_会心5      = 990,  // 幽月轮, 会心提高5%
    幽月轮_伤害3      = 984,  // 幽月轮, 伤害提高3%
    幽月轮_伤害4      = 985,  // 幽月轮, 伤害提高4%
    银月斩_会心3      = 992,  // 银月斩, 会心提高3%
    银月斩_会心4      = 993,  // 银月斩, 会心提高4%
    银月斩_会心5      = 994,  // 银月斩, 会心提高5%
    光明相_调息10_1   = 1029, // 光明相, 调息时间减少10秒
    光明相_调息10_2   = 1030, // 光明相, 调息时间减少10秒
    光明相_调息10_3   = 1031, // 光明相, 调息时间减少10秒
    驱夜断愁_会心4    = 1055, // 驱夜断愁, 会心提高4%
    驱夜断愁_会心5    = 1056, // 驱夜断愁, 会心提高5%
    驱夜断愁_伤害4    = 1052, // 驱夜断愁, 伤害提高4%
    驱夜断愁_伤害5    = 1053, // 驱夜断愁, 伤害提高5%
};

enum skillAlias {
    日轮 = 赤日轮,
    月轮 = 幽月轮,
    日斩 = 烈日斩,
    月斩 = 银月斩,
    日破 = 净世破魔击,
    月破 = 净世破魔击,
    日劫 = 生死劫,
    月劫 = 生死劫,
    光明 = 光明相,
    隐身 = 暗尘弥散,
    驱夜 = 驱夜断愁,
    诛邪 = 诛邪镇魔,
    日悬 = 悬象著明,
    月悬 = 悬象著明,
    崇光 = 崇光斩恶,
};

// clang-format off

static const std::vector<int> cg {
    日斩, 月斩,

    月悬, 日劫, 隐身, 月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    日斩, 驱夜, 日破, 月斩, 月劫, 崇光, 崇光, 崇光, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 诛邪,
    日斩, 驱夜, 日破, 月斩, 月劫, 崇光, 崇光, 崇光, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 诛邪,
    月斩, 日斩, 驱夜,
    月悬, 日劫, 隐身, 月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    日斩, 崇光, 崇光, 崇光, 驱夜, 日破, 月斩, 月劫, 崇光, 崇光, 崇光, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 诛邪,
    日斩, 驱夜, 日破, 月斩, 月劫, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 崇光, 崇光, 崇光, 诛邪,
    月斩, 日斩, 驱夜,

    月悬, 日劫, 隐身, 月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    日斩, 驱夜, 日破, 月斩, 月劫, 崇光, 崇光, 崇光, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 诛邪,
    日斩, 驱夜, 日破, 月斩, 月劫, 崇光, 崇光, 崇光, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 诛邪,
    月斩, 日斩, 驱夜,
    月悬, 日劫, 隐身, 月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    日斩, 崇光, 崇光, 崇光, 驱夜, 日破, 月斩, 月劫, 崇光, 崇光, 崇光, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 诛邪,
    日斩, 驱夜, 日破, 月斩, 月劫, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 崇光, 崇光, 崇光, 诛邪,
    月斩, 日斩, 驱夜,

    月悬, 日劫, 隐身, 月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    日斩, 驱夜, 日破, 月斩, 月劫, 崇光, 崇光, 崇光, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 诛邪,
    日斩, 驱夜, 日破, 月斩, 月劫, 崇光, 崇光, 崇光, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 诛邪,
    月斩, 日斩, 驱夜,
    月悬, 日劫, 隐身, 月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    日斩, 崇光, 崇光, 崇光, 驱夜, 日破, 月斩, 月劫, 崇光, 崇光, 崇光, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 诛邪,
    日斩, 驱夜, 日破, 月斩, 月劫, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 崇光, 崇光, 崇光, 诛邪,
    月斩, 日斩, 驱夜,

    月悬, 日劫, 隐身, 月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    日斩, 崇光, 崇光, 崇光,
};

static const std::vector<int> qg {
    月斩, 月破, 日破, 日斩,

    隐身, 日悬, 月劫, 诛邪, 驱夜, 月斩, 月破, 诛邪, 日破, 诛邪,
    驱夜, 月破, 诛邪, 月斩, 日斩, 日破, 月斩, 光明相, 月劫, 日破, 诛邪,
    驱夜, 日斩, 日破, 月斩, 月破, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月轮, 月劫, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月斩, 月破, 日斩,
    隐身, 日悬, 月劫, 诛邪, 驱夜, 月斩, 月破, 诛邪, 日破, 诛邪,
    驱夜, 月破, 诛邪, 月斩, 日轮, 日破, 月斩, 月劫, 诛邪,
    驱夜, 日斩, 日破, 月斩, 月破, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月轮, 月劫, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月斩, 月破, 日斩,

    隐身, 日悬, 月劫, 诛邪, 驱夜, 月斩, 月破, 诛邪, 日破, 诛邪,
    驱夜, 月破, 诛邪, 月斩, 日斩, 日破, 月斩, 光明相, 月劫, 日破, 诛邪,
    驱夜, 日斩, 日破, 月斩, 月破, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月轮, 月劫, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月斩, 月破, 日斩,
    隐身, 日悬, 月劫, 诛邪, 驱夜, 月斩, 月破, 诛邪, 日破, 诛邪,
    驱夜, 月破, 诛邪, 月斩, 日轮, 日破, 月斩, 月劫, 诛邪,
    驱夜, 日斩, 日破, 月斩, 月破, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月轮, 月劫, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月斩, 月破, 日斩,

    隐身, 日悬, 月劫, 诛邪, 驱夜, 月斩, 月破, 诛邪, 日破, 诛邪,
    驱夜, 月破, 诛邪, 月斩, 日斩, 日破, 月斩, 光明相, 月劫, 日破, 诛邪,
    驱夜, 日斩, 日破, 月斩, 月破, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月轮, 月劫, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月斩, 月破, 日斩,
    隐身, 日悬, 月劫, 诛邪, 驱夜, 月斩, 月破, 诛邪, 日破, 诛邪,
    驱夜, 月破, 诛邪, 月斩, 日轮, 日破, 月斩, 月劫, 诛邪,
    驱夜, 日斩, 日破, 月斩, 月破, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月轮, 月劫, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月斩, 月破, 日斩,

    隐身, 日悬, 月劫, 诛邪, 驱夜, 月斩, 月破, 诛邪, 日破, 诛邪,
    驱夜, 月破, 诛邪, 月斩, 日斩, 日破,
};

static const std::vector<int> outset {
    月斩, 月破, 日破, 日斩,
};

// clang-format on

class MjFysj : public frame::Player {
public:
    MjFysj()
        : Player(10242, 13, 503) {}

private:
    int  idx        = 0;
    bool flagOutset = true;

    void embed0() {
        if (skillGetLevel(37337)) { // 崇光
            if (idx == 0) [[unlikely]]
                stopInitiative.emplace(false);
            if (idx % 99 == 61) {                         // 特效腰坠
                itemUse(frame::ItemType::Trinket, 38789); // 吹香雪
                itemUse(frame::ItemType::Trinket, 39853); // 梧桐影
            }

            // 劫后 400ms 隐身, 多 1 降
            if (idx + 1 < cg.size() && cg[idx + 1] == 隐身)
                delayCustom = 400;

            if (cast(cg[idx])) {
                idx++;
            } else {
                auto tick = skillCooldownLeftTick(cg[idx]);
                if (tick > 0)
                    delayCustom = tick + delayBase + delayRand / 2;
                else
                    stopInitiative.reset();
            }
            if (idx == cg.size()) [[unlikely]]
                stopInitiative.emplace(true);

        } else if (skillGetLevel(34370)) { // 齐光
            if (idx == 0) [[unlikely]]
                stopInitiative.emplace(false);
            if (idx % 100 == 6) {                         // 特效腰坠
                itemUse(frame::ItemType::Trinket, 38789); // 吹香雪
                itemUse(frame::ItemType::Trinket, 39853); // 梧桐影
            }

            // 日斩冷却时间过长, 等一下再打破, 防止掉诛邪 buff
            if (idx + 2 < qg.size() && qg[idx + 2] == 隐身) {
                auto tick = skillCooldownLeftTick(qg[idx + 1]); // 获取日斩的剩余冷却时间
                auto t    = 1024 + delayBase + delayRand;
                if (tick > t) { // 假如剩余冷却时间大于 1 秒以上
                    delayCustom = tick - t;
                    return; // 直接返回, 不进行后续操作
                }
            }

            if (cast(qg[idx])) {
                idx++;
            } else {
                auto tick = skillCooldownLeftTick(qg[idx]);
                if (tick > 0)
                    delayCustom = tick + delayBase + delayRand / 2;
                else
                    stopInitiative.reset();
            }
            if (idx == qg.size()) [[unlikely]]
                stopInitiative.emplace(true);

        } else [[unlikely]] {
            embedStat = 1;
        }
    }

    void embed1() {
        if (flagOutset) {
            cast(outset[idx]);
            idx++;
            if (idx == outset.size()) {
                flagOutset = false;
            }
        } else [[likely]] {
            auto lj = buffGet(28196, 1);
            auto cg = buffGet(28194, 1);
            auto ze = buffGet(28195, 1);
            if (lj && lj->isValid && lj->nStackNum < 3)
                cast(37335); // 崇光斩恶, 连击
            if (cg && cg->isValid && cg->nStackNum == 5 && ze && ze->isValid)
                cast(37335); // 崇光斩恶, 5.5 层
            cast(3974);      // 暗尘弥散
            if (buffExist(25731, 1))
                cast(34347); // 悬象著明
            cast(22890);     // 诛邪镇魔
            cast(3969);      // 光明相
            cast(3966);      // 生死劫
            cast(3967);      // 净世破魔击
            if (cg && cg->isValid && cg->nStackNum >= 3)
                cast(37335); // 崇光斩恶, 3 层
            cast(3979);      // 驱夜断愁
            cast(3963);      // 烈日斩
            cast(3960);      // 银月斩
            cast(3962);      // 赤日轮
        }
    }

    virtual void fightPrepare() override {
        cast(3974);
        if (nSunPowerValue == 0 && nMoonPowerValue == 0) {
            if (nCurrentMoonEnergy >= 10000)
                nMoonPowerValue = 1;
            else if (nCurrentSunEnergy >= 10000)
                nSunPowerValue = 1;
        }
    }
    virtual auto fightWeaponAttack() -> frame::event_tick_t override {
        skillCast(targetSelect, 4326, 1); // 大漠刀法
        int frame = 16 * 1024 / (1024 + this->chAttr.getHaste());
        return frame * 64; // 64 = 1024/16
    }
    virtual void fightEmbed() override {
        if (embedStat == 0) {
            embed0();
        } else if (embedStat == 1) {
            embed1();
        }
    }

    enum class EmbedType {
        Custom = -1,
        CG69   = 0, // 默认循环为 崇光69
        QG30,       // 齐光30耗
        CG699,      // 崇光699
        QG35,       // 齐光35耗
    };

    virtual auto getSkills(const typeSkillMap &custom) -> typeSkillMap override {
        static const typeSkillMap skill{
            {赤日轮, {赤日轮, 33, {赤日轮_会心4, 赤日轮_伤害3, 赤日轮_伤害4, 赤日轮_伤害5}}},
            {烈日斩, {烈日斩, 32, {烈日斩_会心4, 烈日斩_伤害4, 烈日斩_伤害5, 烈日斩_静止10}}},
            {生死劫, {生死劫, 1, {生死劫_伤害3, 生死劫_伤害4, 生死劫_伤害5}}},
            {净世破魔击, {净世破魔击, 32, {净世破魔击_会心5, 净世破魔击_伤害4, 净世破魔击_伤害5, 净世破魔击_20月魂}}},
            {幽月轮, {幽月轮, 24, {幽月轮_会心4, 幽月轮_会心5, 幽月轮_伤害3, 幽月轮_伤害4}}},
            {银月斩, {银月斩, 18, {银月斩_会心3, 银月斩_会心4, 银月斩_会心5}}},
            {光明相, {光明相, 1, {光明相_调息10_1, 光明相_调息10_2, 光明相_调息10_3}}},
            {暗尘弥散, {暗尘弥散, 1, {}}},
            {驱夜断愁, {驱夜断愁, 29, {驱夜断愁_会心4, 驱夜断愁_会心5, 驱夜断愁_伤害4, 驱夜断愁_伤害5}}},
            {诛邪镇魔, {诛邪镇魔, 1, {}}},
        };
        static const typeSkillMap skillForced{
            {净世破魔击, {净世破魔击, 0, {净世破魔击_20月魂}}},
        };
        return overrideSkill(overrideSkill(skill, custom), skillForced);
    }
    virtual auto getTalents(const typeTalentArray &custom) -> typeTalentArray override {
        static const typeTalentArray talent{腾焰飞芒_奇穴, 净身明礼_奇穴, 诛邪镇魔_奇穴, 无明业火_奇穴, 明光恒照_奇穴, 日月同辉_奇穴, 靡业报劫_奇穴, 用晦而明_奇穴, 净体不畏_奇穴, 降灵尊_奇穴, 悬象著明_奇穴, 崇光斩恶_奇穴};
        static const typeTalentArray talentCG{0, 0, 诛邪镇魔_奇穴, 0, 0, 日月同辉_奇穴, 靡业报劫_奇穴, 0, 净体不畏_奇穴, 降灵尊_奇穴, 悬象著明_奇穴, 崇光斩恶_奇穴};
        static const typeTalentArray talentQG{0, 0, 诛邪镇魔_奇穴, 0, 0, 日月同辉_奇穴, 靡业报劫_奇穴, 0, 净体不畏_奇穴, 降灵尊_奇穴, 悬象著明_奇穴, 日月齐光_奇穴};
        typeTalentArray              temp = overrideTalent(talent, custom);
        switch (static_cast<int>(this->embedStat)) {
        case static_cast<int>(EmbedType::CG69):
            return overrideTalent(temp, talentCG);
        case static_cast<int>(EmbedType::QG30):
            return overrideTalent(temp, talentQG);
        case static_cast<int>(EmbedType::CG699):
            return overrideTalent(temp, talentCG);
        case static_cast<int>(EmbedType::QG35):
            return overrideTalent(temp, talentQG);
        default:
            return temp;
        }
    }
};
} // namespace

template <>
auto concrete::create<player::Type::MjFysj>() -> std::unique_ptr<frame::Player> {
    return std::make_unique<MjFysj>();
}
