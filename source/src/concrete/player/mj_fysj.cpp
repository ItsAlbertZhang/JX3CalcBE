#include "concrete.h"
#include "frame/character/derived/player.h"
#include "frame/common/event.h"
#include "frame/event.h"

using namespace jx3calc;

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
                stopInitiative.emplace(1);
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
                stopInitiative.emplace(0);

        } else if (skillGetLevel(34370)) { // 齐光
            if (idx == 0) [[unlikely]]
                stopInitiative.emplace(1);
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
                stopInitiative.emplace(0);

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
            embed上限_崇光69();
        } else if (embedStat == 1) {
            embed1();
        }
    }

    enum class EmbedType {
        自定义      = -1,
        // 上限循环
        上限_崇光69 = 0, // 默认
        上限_齐光30,
        上限_崇光699,
        上限_齐光35,
        // 简单循环
        简单_齐光一键 = 1024,
        简单_崇光两键,
        简单_齐光两键,
        简单_崇光洞若,
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
        case static_cast<int>(EmbedType::上限_崇光69):
            return overrideTalent(temp, talentCG);
        case static_cast<int>(EmbedType::上限_齐光30):
            return overrideTalent(temp, talentQG);
        case static_cast<int>(EmbedType::上限_崇光699):
            return overrideTalent(temp, talentCG);
        case static_cast<int>(EmbedType::上限_齐光35):
            return overrideTalent(temp, talentQG);
        default:
            return temp;
        }
    }

    int  ava日斩 = 1;
    int  ava月斩 = 1;
    int  ava诛邪 = 1;
    void embed上限_崇光69() {
        // 获取战斗时间
        const auto now = frame::Event::now();
        if (now < 1024)
            stopInitiative.emplace(1);
        if (now > fightTick * 2) // 调试用, 技能卡死
            stopInitiative.emplace(0);
        // 起手双斩
        const auto buff目标日斩 = targetSelect ? targetSelect->buffGet(4418, 1) : nullptr;
        const auto buff目标月斩 = targetSelect ? targetSelect->buffGet(4202, 0) : nullptr;
        const auto time目标日斩 = buff目标日斩 && buff目标日斩->isValid ? buff目标日斩->nLeftFrame * 1024 / 16 : 0;
        const auto time目标月斩 = buff目标月斩 && buff目标月斩->isValid ? buff目标月斩->nLeftFrame * 1024 / 16 : 0;
        if (time目标日斩 == 0 && now < 2 * 1024)
            cast(烈日斩);
        if (time目标月斩 == 0 && now < 2 * 1024)
            cast(银月斩);
        // 崇光
        const auto buff降灵尊   = buffGet(25731, 1);
        const auto buff悬象     = buffGet(25716, 0);
        const auto buff崇光     = buffGet(28194, 1);
        const auto buff斩恶     = buffGet(28195, 1);
        const auto buff连击     = buffGet(28196, 1);
        const auto stacknum崇光 = buff崇光 && buff崇光->isValid ? buff崇光->nStackNum : 0;
        const auto stacknum连击 = buff连击 && buff连击->isValid ? buff连击->nStackNum : 0;
        const auto cd生死劫     = skillCooldownLeftTick(生死劫);
        const auto cd隐身       = skillCooldownLeftTick(隐身);
        const auto cd悬象       = skillCooldownLeftTick(悬象著明);
        const auto buff橙武CD   = buffGet(2584, 0);
        const auto time橙武CD   = buff橙武CD && buff橙武CD->isValid ? buff橙武CD->nLeftFrame * 1024 / 16 : 0;
        const bool bool崇悬前 =
            cd悬象 < 5 * 1024 && stacknum崇光 >= 3; // 悬象CD小于5s, 并且拥有至少3层崇光
        const bool bool崇劫后 =
            cd生死劫 > (14 - 1) * 1024 - delayBase - delayRand && // 刚打完生死劫, 并且
            time橙武CD < 25 * 1024 &&                             // 没有橙武特效, 并且
            stacknum崇光 >= 3 &&                                  // 拥有至少3层崇光, 并且
            cd隐身 > 0 &&                                         // 隐身在冷却中, 并且
            !(buff悬象 && buff悬象->isValid);                     // 没有悬象
        const bool bool崇降后 =
            time目标日斩 > 16 * 1024 && stacknum崇光 >= 5 && // 目标日斩buff时长大于16秒, 并且拥有5层崇光, 并且
            !(buff降灵尊 && buff降灵尊->isValid);            // 没有降灵尊
        const bool bool崇溢出 =
            time目标日斩 > 3 * (960 + delayBase + delayRand) && // 目标日斩buff时长大于3秒, 并且
            stacknum崇光 >= 5 && buff斩恶 && buff斩恶->isValid; // 拥有5.5层崇光
        const bool bool崇连击 =
            stacknum连击 > 0 && stacknum连击 < 3; // 有连击但是不满3层(满3层时下一次崇光就会被重置)
        if (bool崇悬前 || bool崇劫后 || bool崇降后 || bool崇连击) {
            cast(崇光斩恶);
            if (stacknum连击 == 2 && stopInitiative.value() == 2)
                stopInitiative.emplace(0);
            return;
        }
        // 隐身. 隐含条件: 没有成功释放崇光 (如果成功释放崇光, 会在此前返回)
        if (buff悬象 && buff悬象->isValid &&                              // 有悬象buff, 并且满足以下两者之一:
            ((nCurrentSunEnergy < 10000 && nCurrentMoonEnergy < 10000) || // 1. 日灵月魂均小于100
             time橙武CD > 25 * 1024 + 960 + delayBase + delayRand)        // 2. 在橙武特效中, 且不在最后一秒
        ) {
            if (cast(暗尘弥散))
                return;
        }

        // 处理特效腰坠
        if (buff降灵尊 && buff降灵尊->isValid &&                        // 降灵尊内, 并且
            nCurrentSunEnergy >= 10000 && nCurrentMoonEnergy == 8000 && // 100/80, 并且
            stacknum崇光 >= 4                                           // 拥有至少4层崇光
        ) {
            itemUse(frame::ItemType::Trinket, 39853); // 梧桐影
            itemUse(frame::ItemType::Trinket, 38789); // 吹香雪
        }

        // 满灵部分
        // 悬象
        if (cd隐身 < 2 * 1024 &&                                            // 隐身cd小于5秒, 并且满足以下二者其一:
            ((nCurrentMoonEnergy >= 10000 && nCurrentSunEnergy >= 10000) || // 1. 在双满状态下
             time橙武CD > 25 * 1024 + 960 + delayBase + delayRand)          // 2. 在橙武特效中, 且不在最后一秒
        ) {
            if (stacknum崇光 == 2 && buff斩恶 && buff斩恶->isValid && time橙武CD < 25 * 1024)
                buffDelGroup(28195, 1); // 悬前2.5崇且不在cw特效, 点掉0.5崇
            if (time橙武CD > 25)
                buffDelGroup(25765, 1); // 特效内则点掉灵魂, 防止溢出
            if (time目标日斩 < 14 * 1024 && time橙武CD < 25 * 1024)
                cast(烈日斩); // 特效内不补斩
            if (time目标月斩 < 12 * 1024 && time橙武CD < 25 * 1024)
                cast(银月斩); // 特效内不补斩
            cast(悬象著明);
        }
        // 悬象劫. 与悬象绑定.
        if (buff悬象 && buff悬象->isValid)                    // 有悬象buff
            if ((nMoonPowerValue && buff悬象->nLevel == 1) || // 满月且有日悬, 或者
                (nSunPowerValue && buff悬象->nLevel == 2))    // 满日且有月悬
                if (cast(生死劫))
                    delayCustom = 400;
        // 普通劫. 有半边诛邪或者有整个诛邪都可以放.
        const auto buff灵日 = buffGet(9910, 0);
        const auto buff魂月 = buffGet(9911, 0);
        const auto time半边诛邪 =
            buff灵日 && buff灵日->isValid
                ? buff灵日->nLeftFrame * 1024 / 16
            : buff魂月 && buff魂月->isValid
                ? buff魂月->nLeftFrame * 1024 / 16
                : 0;
        const auto buff诛邪 = buffGet(9909, 0);
        if ((time半边诛邪 > 0 || (buff诛邪 && buff诛邪->isValid)) && cd隐身 > 6 * 1024)
            cast(生死劫);
        // 赶时间劫. 如果隐身cd对14取余大于11, 则直接放. (隐前劫cd转好时, 隐身已经转好0~3秒)
        // if (cd隐身 % (14 * 1024) > (11 * 1024))
        //     cast(生死劫);
        // 特殊处理: 崇光填充.
        if (stacknum崇光 >= 3 &&                                           // 拥有至少3层崇光, 并且
            (nCurrentSunEnergy >= 10000 || nCurrentMoonEnergy >= 10000) && // 手中有满灵, 并且
            cd生死劫 < 3 * 1024 &&                                         // 劫CD小于3s, 并且
            time橙武CD < 25 * 1024 &&                                      // 不在橙武特效中, 并且
            (time半边诛邪 > 3 * 1024 || time橙武CD > 23 * 1024)            // 半边诛邪持续时间大于3秒或刚结束橙武特效
        )
            cast(崇光斩恶);
        // 诛邪. 不在cw特效里放, 不在劫cd好时放. 注意: 需要 ava诛邪 (控制诛邪释放的标记) 为 1.
        if (time橙武CD < 25 * 1024 && cd生死劫 > 0 && ava诛邪 > 0)
            cast(诛邪镇魔);
        // 破魔击. 直接放.
        cast(净世破魔击);

        const bool bool崇收尾 =
            now - fightTick > -20 * 1024 &&                     // 距离预定的收尾时间不到20s, 或已经超过了预定的收尾时间, 并且
            cd隐身 > 25 * 1024 && stacknum崇光 >= 3 &&          // 隐身CD大于25s且拥有3层崇光, 并且
            time目标日斩 > 3 * (960 + delayBase + delayRand) && // 目标日斩buff时长大于3个GCD
            !(buff降灵尊 && buff降灵尊->isValid);               // 不在降内
        if (bool崇收尾) {
            cast(崇光斩恶);
            stopInitiative.emplace(2);
        }

        // 处理卡驱夜CD的延迟诛邪
        if (buff降灵尊 && buff降灵尊->isValid && nCurrentSunEnergy == 0 && nCurrentMoonEnergy == 4000)
            ava诛邪 = 0; // 暂时停止诛邪释放
        // 处理离开cw特效的情况
        if (time橙武CD > 24 * 1024 && time橙武CD < 25 * 1024) {
            // 此时橙武特效刚结束, 手中应该还有一个满灵
            const auto cnt三崇 = cd隐身 / (14 * 1024) + (cd生死劫 == 0 ? 1 : 0); // 一个生死劫插三崇
            const auto cnt额外 = (cd隐身 - 3 * cnt三崇 * 1024) % (6 * 1024);
            // 破诛异驱破破诛 - 双满
            // 破诛同破 - 单边满
            // 破驱诛破斩驱破诛破 - 双满+单边满
            if (cnt额外 > 2 * 1024 && cnt额外 < 5 * 1024) {
                const auto buff同辉 = buffGet(12850, 2);
                const auto buff灵魂 = buffGet(25765, 1);
                const auto cnt灵魂 =
                    (buff同辉 && buff同辉->isValid ? buff同辉->nStackNum * 5 : 0) +
                    (buff灵魂 && buff灵魂->isValid ? buff灵魂->nStackNum : 0);
                const auto cnt小节 = (cd隐身 - 3 * cnt三崇 * 1024) / (6 * 1024);
                if (cnt额外 > 3 * 1024 && cnt灵魂 > cnt小节 * 2) {
                    // 单边满
                    if (nCurrentSunEnergy < 10000) {
                        ava日斩 = 1;
                        ava月斩 = 0;
                    } else if (nCurrentMoonEnergy < 10000) {
                        ava日斩 = 0;
                        ava月斩 = 1;
                    }
                } else {
                    // 双满+单边满
                    if (nCurrentSunEnergy == 10000) {
                        ava日斩 = 1;
                        ava月斩 = 0;
                    } else if (nCurrentMoonEnergy == 10000) {
                        ava日斩 = 0;
                        ava月斩 = 1;
                    }
                }
            } else {
                // 双满
                if (nCurrentSunEnergy < 10000) {
                    ava日斩 = 0;
                    ava月斩 = 1;
                } else if (nCurrentMoonEnergy < 10000) {
                    ava日斩 = 1;
                    ava月斩 = 0;
                }
            }
        }
        // 无灵魂时, 重置日月斩和驱夜
        if (nCurrentSunEnergy <= 2000 && nCurrentMoonEnergy <= 4000) {
            ava日斩 = 1;
            ava月斩 = 1;
        }

        // 双斩.
        const auto cd驱夜 = skillCooldownLeftTick(驱夜断愁);
        if (!(buff悬象 && buff悬象->isValid)) {
            // 悬外:
            // 1. 双方都可用时, 随便放.
            // 2. 自身可用且悬冷却完毕时, 可以放.
            // 3. 自身灵>=60, 对方灵<=20, 且驱夜在CD或有对半边诛邪时, 可以放.
            if (ava日斩 > 0 && ava月斩 > 0) {
                if (time目标日斩 > 6 * 1024 && time目标月斩 < 6 * 1024) {
                    ava月斩 -= cast(银月斩) ? 1 : 0;
                    ava日斩 -= cast(烈日斩) ? 1 : 0;
                } else {
                    ava日斩 -= cast(烈日斩) ? 1 : 0;
                    ava月斩 -= cast(银月斩) ? 1 : 0;
                }
            } else if (ava日斩 > 0 && cd悬象 == 0 && nCurrentSunEnergy < 4000) {
                ava日斩 -= cast(烈日斩) ? 1 : 0;
            } else if (ava月斩 > 0 && cd悬象 == 0 && nCurrentMoonEnergy < 4000) {
                ava月斩 -= cast(银月斩) ? 1 : 0;
            } else if (nCurrentSunEnergy >= 6000 && nCurrentMoonEnergy <= 2000 && (ava日斩 > 0 || cd驱夜 > 0 || (buff魂月 && buff魂月->isValid))) {
                ava日斩 -= cast(烈日斩) ? 1 : 0;
            } else if (nCurrentMoonEnergy >= 6000 && nCurrentSunEnergy <= 2000 && (ava月斩 > 0 || cd驱夜 > 0 || (buff灵日 && buff灵日->isValid))) {
                ava月斩 -= cast(银月斩) ? 1 : 0;
            }
        } else if (nCurrentSunEnergy >= 8000 && nCurrentMoonEnergy >= 6000) {
            // 悬内: 80/80, 放月斩
            ava月斩 -= cast(银月斩) ? 1 : 0;
        } else if (nCurrentSunEnergy >= 6000 && nCurrentMoonEnergy >= 8000) {
            // 悬内: 60/80, 放日斩
            ava日斩 -= cast(烈日斩) ? 1 : 0;
        }
        // 驱夜. 驱夜释放完毕后, 将诛邪标记恢复.
        if (cast(驱夜断愁)) {
            ava诛邪 = 1;
        }
        // 最后, 如果什么技能都放不出来, 那么尝试放一个诛邪当填充技.
        cast(诛邪镇魔);
    }
};
} // namespace

template <>
auto concrete::createPlayer<concrete::Player::MjFysj>() -> std::unique_ptr<frame::Player> {
    return std::make_unique<MjFysj>();
}
