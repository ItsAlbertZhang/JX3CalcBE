#include "concrete.h"
#include "frame/character/derived/player.h"
#include "frame/common/event.h"
#include "frame/event.h"

using namespace jx3calc;

namespace {

class MjFysj : public frame::Player {
public:
    MjFysj();

private:
    virtual auto getSkills(const typeSkillMap &custom) -> typeSkillMap override;
    virtual auto getTalents(const typeTalentArray &custom) -> typeTalentArray override;

    virtual void fightPrepare() override;
    virtual auto fightWeaponAttack() -> frame::event_tick_t override;
    virtual void fightEmbed() override;

    int framePublicCooldown = 0;

    enum class EmbedType {
        自定义      = -1,
        // 上限循环
        上限_崇光69 = 0,
        // 严格循环
        严格_崇光69 = 1 << 5,
        严格_崇光699,
        严格_齐光30,
        严格_齐光35,
        // 简单循环
        简单_齐光一键 = 1 << 10,
        简单_崇光两键,
        简单_崇光洞若,
    };

    int  nFight = 0;
    bool bFight = true;
    void fightStrict(const std::vector<int> &queue);

    void fightBurst();
    void fightConv();    // conventional fight
    void fightConvVar(); // variant conventional fight
    void fightCW();

    void fight_上限_崇光69();
    void fight_简单_齐光一键();
    void fight_简单_崇光两键();
    void fight_简单_崇光洞若();
};
} // namespace

template <>
auto concrete::createPlayer<concrete::Player::MjFysj>() -> std::unique_ptr<frame::Player> {
    return std::make_unique<MjFysj>();
}

namespace {

// 使用 C 风格的 enum, 以便直接使用

enum talent {
    腾焰飞芒_奇穴 = 5972,
    净身明礼_奇穴 = 18279,
    诛邪镇魔_奇穴 = 22888,
    洞若观火_奇穴 = 5978,
    无明业火_奇穴 = 6717,
    明光恒照_奇穴 = 34383,
    日月同辉_奇穴 = 34395,
    靡业报劫_奇穴 = 34372,
    用晦而明_奇穴 = 17567,
    净体不畏_奇穴 = 25166,
    天地诛戮_奇穴 = 5979,
    降灵尊_奇穴   = 34378,
    悬象著明_奇穴 = 34347,
    崇光斩恶_奇穴 = 37337,
    日月齐光_奇穴 = 34370,
};

const std::set<int> talentAvailable {
    腾焰飞芒_奇穴,
    净身明礼_奇穴,
    诛邪镇魔_奇穴,
    洞若观火_奇穴,
    无明业火_奇穴,
    明光恒照_奇穴,
    日月同辉_奇穴,
    靡业报劫_奇穴,
    用晦而明_奇穴,
    净体不畏_奇穴,
    天地诛戮_奇穴,
    降灵尊_奇穴,
    悬象著明_奇穴,
    崇光斩恶_奇穴,
    日月齐光_奇穴,
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

enum operation {
    特效腰坠 = -1024,
    延迟隐身,
    点诛,
    特殊处理1,
};

MjFysj::MjFysj() :
    Player(8, 10242, 13, 503) {}

auto MjFysj::getSkills(const typeSkillMap &custom) -> typeSkillMap {
    static const typeSkillMap skill {
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
    static const typeSkillMap skillForced {
        {净世破魔击, {净世破魔击, 0, {净世破魔击_20月魂}}},
    };
    return overrideSkill(overrideSkill(skill, custom), skillForced);
}

auto MjFysj::getTalents(const typeTalentArray &custom) -> typeTalentArray {
    static const typeTalentArray talent {腾焰飞芒_奇穴, 净身明礼_奇穴, 诛邪镇魔_奇穴, 无明业火_奇穴, 明光恒照_奇穴, 日月同辉_奇穴, 靡业报劫_奇穴, 用晦而明_奇穴, 净体不畏_奇穴, 降灵尊_奇穴, 悬象著明_奇穴, 崇光斩恶_奇穴};
    static const typeTalentArray talentCG {0, 0, 诛邪镇魔_奇穴, 0, 0, 日月同辉_奇穴, 靡业报劫_奇穴, 0, 净体不畏_奇穴, 降灵尊_奇穴, 悬象著明_奇穴, 崇光斩恶_奇穴};
    static const typeTalentArray talentQG {0, 0, 诛邪镇魔_奇穴, 0, 0, 日月同辉_奇穴, 靡业报劫_奇穴, 0, 净体不畏_奇穴, 降灵尊_奇穴, 悬象著明_奇穴, 日月齐光_奇穴};

    typeTalentArray temp = overrideTalent(talent, custom);
    for (int i = 0; i < CountTalents; i++)
        if (!talentAvailable.contains(temp[i]))
            temp[i] = talent[i];

    switch (static_cast<int>(fightType)) {
    case static_cast<int>(EmbedType::上限_崇光69):
        return overrideTalent(temp, talentCG);
    case static_cast<int>(EmbedType::严格_崇光69):
        return overrideTalent(temp, talentCG);
    case static_cast<int>(EmbedType::严格_崇光699):
        return overrideTalent(temp, talentCG);
    case static_cast<int>(EmbedType::严格_齐光30):
        return overrideTalent(temp, talentQG);
    case static_cast<int>(EmbedType::严格_齐光35):
        return overrideTalent(temp, talentQG);
    default:
        return temp;
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

extern const std::vector<int> queue崇光69;
extern const std::vector<int> queue崇光699;
extern const std::vector<int> queue齐光30;
extern const std::vector<int> queue齐光35;

void MjFysj::fightEmbed() {
    switch (static_cast<int>(fightType)) {
    case static_cast<int>(EmbedType::上限_崇光69):
        return fight_上限_崇光69();
    case static_cast<int>(EmbedType::严格_崇光69):
        return fightStrict(queue崇光69);
    case static_cast<int>(EmbedType::严格_崇光699):
        return fightStrict(queue崇光699);
    case static_cast<int>(EmbedType::严格_齐光30):
        return fightStrict(queue齐光30);
    case static_cast<int>(EmbedType::严格_齐光35):
        return fightStrict(queue齐光35);
    case static_cast<int>(EmbedType::简单_齐光一键):
        return fight_简单_齐光一键();
    case static_cast<int>(EmbedType::简单_崇光两键):
        return fight_简单_崇光两键();
    case static_cast<int>(EmbedType::简单_崇光洞若):
        return fight_简单_崇光洞若();
    default:
        return fight_简单_齐光一键();
    }
}

void MjFysj::fightStrict(const std::vector<int> &queue) {
    if (nFight == 0) [[unlikely]]
        fightStopWait.emplace(1);
    if (nFight >= queue.size()) [[unlikely]]
        fightStopWait.emplace(0);
    if (queue[nFight] < 0) {
        switch (queue[nFight]) {
        case 特效腰坠: {
            itemUse(frame::ItemType::Trinket, 39853); // 梧桐影
            itemUse(frame::ItemType::Trinket, 38789); // 吹香雪
            nFight++;
        } break;
        case 延迟隐身: {
            const auto cd = skillCooldownLeftTick(暗尘弥散);
            if (cd < 400) {
                delayCustom = 400;
                nFight++;
            } else {
                delayCustom = static_cast<int>(cd - 400 + delayBase + delayRand / 2);
                return;
            }
        } break;
        case 点诛: {
            buffDelMultiGroupByID(9909);
            nFight++;
        } break;
        case 特殊处理1: {
            const auto cd = skillCooldownLeftTick(烈日斩); // 获取日斩的剩余冷却时间
            const auto t  = framePublicCooldown * 1024 / 16 + delayBase + delayRand;
            if (cd > t) { // 假如剩余冷却时间大于 1 秒以上
                delayCustom = static_cast<int>(cd - t);
                return; // 直接返回, 不进行后续操作
            } else {
                nFight++;
            }
        } break;
        }
    }
    const auto &skill = queue[nFight];
    if (cast(skill)) {
        nFight++;
    } else {
        const auto tick = skillCooldownLeftTick(skill);
        if (tick > 0)
            return;
        else
            fightStopWait.reset();
    }
    if (nFight == queue.size()) [[unlikely]]
        fightStopWait.emplace(0);
}

void MjFysj::fightBurst() {
    // 进入此小节, 隐身一定冷却完毕, 最起码冷却时间小于1.5秒.
    // 悬象一定冷却完毕, 因为上一次悬象的释放时机一定在: 隐身前2秒~隐身后瞬间.

    // 后续计算所需资源
    const auto buff悬象   = buffGet(25716, 0);
    const auto buff崇光   = buffGet(28194, 1);
    const auto buff橙武CD = buffGet(2584, 0);
    const auto time橙武CD = buff橙武CD && buff橙武CD->isValid ? buff橙武CD->nLeftFrame * 1024 / 16 : 0;
    const auto cd生死劫   = skillCooldownLeftTick(生死劫);
    const auto cd暗尘弥散 = skillCooldownLeftTick(暗尘弥散);

    // 处理特效腰坠
    if (nCurrentSunEnergy >= 10000 && nCurrentMoonEnergy == 8000 && // 100/80, 并且
        buff崇光 && buff崇光->isValid && buff崇光->nStackNum >= 4   // 拥有至少4层崇光
    ) {
        itemUse(frame::ItemType::Trinket, 39853); // 梧桐影
        itemUse(frame::ItemType::Trinket, 38789); // 吹香雪
    }

    // 悬前补斩
    if (skillCountAvailable(烈日斩) == 2 && !(buff悬象 && buff悬象->isValid))
        if (cast(烈日斩))
            return;

    // 施展隐身. 可能在GCD内释放, 也可能在释放其他技能前释放.
    if ((nCurrentSunEnergy < 10000 && nCurrentMoonEnergy < 10000) || // 日灵月魂均小于100, 或
        time橙武CD > 26 * 1024 + delayBase + delayRand)              // 在橙武特效中, 且不在最后一秒
        if (cast(暗尘弥散))
            return;

    // 直接施展悬象
    if (cast(悬象著明))
        return;
    // 施展悬象反色生死劫
    if (buff悬象 && buff悬象->isValid)
        if ((nMoonPowerValue && buff悬象->nLevel == 1) || // 满月且有日悬, 或者
            (nSunPowerValue && buff悬象->nLevel == 2)) {  // 满日且有月悬
            if (cast(生死劫)) {
                if (cd暗尘弥散 == 0)
                    delayCustom = 400;
                return;
            } else if (cd生死劫 < 400) {
                delayCustom = static_cast<int>(cd生死劫 + delayBase + delayRand / 2);
                return;
            }
        }
    // 生死劫在CD且满灵时, 施展诛邪
    if (cd生死劫 > 0 && (nSunPowerValue || nMoonPowerValue))
        if (cast(诛邪镇魔))
            return;
    // 施展破魔击
    if (cast(净世破魔击)) {
        if (skillCooldownLeftTick(暗尘弥散) == 0)
            delayCustom = 400;
        return;
    }
    // 处理一种特殊情况: 还有3层悬象, 而灵魂状态是 x/0.
    // 此时如果直接打驱夜, 会出现问题:
    // (x/0)-驱夜(100/60)-诛邪-日破(0/80,2)-诛邪-驱夜(60/100)-月破(80/20,1)-诛邪(卡住)
    // 在橙武特效中可能出现这样的问题. 例如: 日悬-月破-日破-月劫(x/0) 接上述情况.
    // 此处对其进行特殊处理, 在这种情况下打一个月斩.
    if (buff悬象 && buff悬象->isValid && buff悬象->nStackNum == 3 && nCurrentSunEnergy >= 40 && nCurrentMoonEnergy == 0)
        if (cast(银月斩))
            return;
    // 施展驱夜断愁
    if (cast(驱夜断愁))
        return;
    // 施展诛邪
    if (cast(诛邪镇魔))
        return;
    // 可能的日月斩 (由橙武特效引起)
    if (nCurrentSunEnergy + nCurrentMoonEnergy >= 14000)
        if (cast(nCurrentMoonEnergy > nCurrentSunEnergy ? 烈日斩 : 银月斩))
            return;
}

void MjFysj::fightConv() {
    // 后续计算所需资源
    const auto cd生死劫 = skillCooldownLeftTick(生死劫);
    const auto buff灵日 = buffGet(9910, 0);
    const auto buff魂月 = buffGet(9911, 0);
    const auto time半边诛邪 =
        buff灵日 && buff灵日->isValid
            ? buff灵日->nLeftFrame * 1024 / 16
        : buff魂月 && buff魂月->isValid
            ? buff魂月->nLeftFrame * 1024 / 16
            : 0;
    const auto buff诛邪 = buffGet(9909, 0);
    const auto time诛邪 = buff诛邪 && buff诛邪->isValid ? buff诛邪->nLeftFrame * 1024 / 16 : 0;
    const auto buff同辉 = buffGet(12850, 2);
    const auto buff灵魂 = buffGet(25765, 1);
    const auto count灵魂 =
        (buff同辉 && buff同辉->isValid ? buff同辉->nStackNum * 5 : 0) +
        (buff灵魂 && buff灵魂->isValid ? buff灵魂->nStackNum : 0);
    const auto buff降灵尊 = buffGet(25731, 1);
    // 诛邪
    if (time诛邪 < 1024 + delayBase + delayRand || // 最后一秒, 或者
        nSunPowerValue || nMoonPowerValue ||       // 满灵, 或者
        count灵魂 < 5 ||                           // 灵魂层数小于5 (灵魂大于等于5才能进入双斩驱夜双满小节), 或者
        (buff降灵尊 && buff降灵尊->isValid)        // 降灵尊内
    )
        if (cast(诛邪镇魔))
            return;
    // 生死劫, 有半边诛邪时施展
    if (time半边诛邪 > 0 && skillCooldownLeftTick(隐身) > 6 * 1024 && (nSunPowerValue || nMoonPowerValue)) {
        if (cast(生死劫))
            return;
        else if (cd生死劫 < 400) {
            delayCustom = static_cast<int>(cd生死劫 + delayBase + delayRand / 2);
            return;
        }
    }
    // 破魔击, 无条件施展
    if (cast(净世破魔击))
        return;
    // 空灵斩, 同时获取驱夜断愁标记
    if (nCurrentSunEnergy <= 2000 && nCurrentMoonEnergy <= 4000) {
        bFight = true;
        if (cast(烈日斩))
            return;
        if (cast(银月斩))
            return;
    }
    // 驱夜
    if (bFight || count灵魂 >= 13)
        if (cast(驱夜断愁)) {
            if (count灵魂 < 15) {
                bFight = false;
            }
            return;
        }
    // 高灵斩
    if (nCurrentSunEnergy >= 6000)
        if (cast(烈日斩))
            return;
    if (nCurrentMoonEnergy >= 6000)
        if (cast(银月斩))
            return;
}

void MjFysj::fightConvVar() {
    // 后续计算所需资源
    const auto cd生死劫 = skillCooldownLeftTick(生死劫);
    const auto buff灵日 = buffGet(9910, 0);
    const auto buff魂月 = buffGet(9911, 0);
    const auto time半边诛邪 =
        buff灵日 && buff灵日->isValid
            ? buff灵日->nLeftFrame * 1024 / 16
        : buff魂月 && buff魂月->isValid
            ? buff魂月->nLeftFrame * 1024 / 16
            : 0;
    const auto buff诛邪 = buffGet(9909, 0);
    const auto time诛邪 = buff诛邪 && buff诛邪->isValid ? buff诛邪->nLeftFrame * 1024 / 16 : 0;
    const auto buff同辉 = buffGet(12850, 2);
    const auto buff灵魂 = buffGet(25765, 1);
    const auto count灵魂 =
        (buff同辉 && buff同辉->isValid ? buff同辉->nStackNum * 5 : 0) +
        (buff灵魂 && buff灵魂->isValid ? buff灵魂->nStackNum : 0);
    const auto buff降灵尊 = buffGet(25731, 1);
    // 诛邪
    if (time诛邪 < 1024 + delayBase + delayRand || // 最后一秒, 或者
        nSunPowerValue || nMoonPowerValue ||       // 满灵, 或者
        count灵魂 < 5 ||                           // 灵魂层数小于5 (灵魂大于等于5才能进入双斩驱夜双满小节), 或者
        (buff降灵尊 && buff降灵尊->isValid)        // 降灵尊内
    )
        if (cast(诛邪镇魔))
            return;
    // 生死劫, 有半边诛邪时施展
    if (time半边诛邪 > 0 && skillCooldownLeftTick(隐身) > 6 * 1024 && (nSunPowerValue || nMoonPowerValue)) {
        if (cast(生死劫))
            return;
        else if (cd生死劫 < 400) {
            delayCustom = static_cast<int>(cd生死劫 + delayBase + delayRand / 2);
            return;
        }
    }
    // 破魔击, 无条件施展
    if (cast(净世破魔击))
        return;
    // 空灵双斩
    if (nCurrentSunEnergy <= 2000 || (!(targetSelect->buffExist(4418, 1)) && nCurrentSunEnergy <= 4000))
        if (cast(烈日斩))
            return;
    if (nCurrentMoonEnergy <= 2000 || (!(targetSelect->buffExist(4202, 0)) && nCurrentMoonEnergy <= 4000))
        if (cast(银月斩))
            return;
    // 二段加速下,自适应 Beta
    if (framePublicCooldown <= 14) {
        if (nCurrentSunEnergy >= 4000 && nCurrentMoonEnergy >= 4000) {
            // 单边满灵
            // 破劫诛-3崇-6GCD-6GCD-3崇-6GCD-5GCD = 32GCD, 再给 400ms 的劫停手延迟
            const auto t = 32 * (framePublicCooldown * 1024 / 16 + delayBase + delayRand * 3 / 4) + 400; // 这里多取 1/4 的随机延迟
            if (skillCooldownLeftTick(暗尘弥散) > t) {
                if (cast(银月斩))
                    return;
            }
            // 月轮
            if (nCurrentMoonEnergy == 8000)
                if (cast(幽月轮))
                    return;
        }
        if (skillCountAvailable(烈日斩) == 2 && nCurrentSunEnergy >= 6000)
            if (cast(烈日斩))
                return;
        if (skillCountAvailable(银月斩) == 2 && nCurrentMoonEnergy >= 6000)
            if (cast(银月斩))
                return;
        // 驱夜
        if (cast(驱夜断愁))
            return;
    } else {
        if (nCurrentSunEnergy >= 4000 && nCurrentMoonEnergy >= 4000)
            if (cast(驱夜断愁))
                return;
    }
}

void MjFysj::fightCW() {
    // 后续计算所需资源
    const auto cd生死劫     = skillCooldownLeftTick(生死劫);
    const auto cd暗尘弥散   = skillCooldownLeftTick(暗尘弥散);
    const auto buff悬象     = buffGet(25716, 0);
    const auto buff橙武CD   = buffGet(2584, 0);
    const auto time橙武CD   = buff橙武CD && buff橙武CD->isValid ? buff橙武CD->nLeftFrame * 1024 / 16 : 0;
    const auto buff目标日劫 = buffGet(25725, 0);
    const auto buff目标月劫 = buffGet(25726, 0);
    // 悬象. 可以提前放.
    if (time橙武CD > 27 * 1024 && cd暗尘弥散 < 1024)
        cast(悬象著明); // 提前放悬象
    // 隐身
    if (time橙武CD > 26 * 1024 && cd暗尘弥散 == 0) {
        cast(暗尘弥散); // 一定能放出来
        cast(悬象著明); // 不一定放得出来, 因为有可能悬象提前放了
    }
    // 生死劫
    if (buff悬象 && buff悬象->isValid) {                  // 如果有悬象, 那么必须满足以下二者之一, 才施展:
        if ((nMoonPowerValue && buff悬象->nLevel == 1) || // 1. 满月且有日悬, 或者
            (nSunPowerValue && buff悬象->nLevel == 2)) {  // 2. 满日且有月悬
            // cast(生死劫);
            if (cast(生死劫))
                return;
            else if (cd生死劫 < 400) {
                delayCustom = static_cast<int>(cd生死劫 + delayBase + delayRand / 2);
                return;
            }
        }
    } else { // 否则(如果没有悬象), 可以直接施展
        cast(生死劫);
    }
    // 处理一种特殊情况: 在特效内打诛邪.
    // 2024.5.20 note: 发现不处理对 DPS 没啥影响, 好像还高几百.
    // 暂时注释, 先不处理.
    // if (buff悬象 && buff悬象->isValid && cd生死劫 > 0 &&                                  // 有悬象, 并且生死劫在CD中, 并且
    //     !(buff目标日劫 && buff目标日劫->isValid && buff目标月劫 && buff目标月劫->isValid) // 目标身上没有双buff
    // ) {
    //     const auto 悬象层数   = buff悬象->nStackNum - 1; // 留 1 层容错为异色劫需求
    //     const auto GCD时间    = framePublicCooldown * 1024 / 16 + delayBase + delayRand;
    //     const auto 劫前技能数 = cd生死劫 / GCD时间 + 1;
    //     const auto 连满技能数 = (time橙武CD - 25 * 1024) / GCD时间 + 1;
    //     // 如果悬象层数大于劫前技能数, 那么没有修正的必要
    //     // 如果连满技能数小于劫前技能数, 那么没有修正的可能
    //     const bool intervene  = 悬象层数 <= 劫前技能数 && 连满技能数 >= 劫前技能数;
    //     if (intervene) {
    //         cast(诛邪镇魔);
    //     }
    // }
    cast(净世破魔击);
}

void MjFysj::fight_上限_崇光69() {
    // 后续计算所需资源
    const auto now          = frame::Event::now();
    const auto buff目标日斩 = targetSelect ? targetSelect->buffGet(4418, 1) : nullptr;
    const auto buff目标月斩 = targetSelect ? targetSelect->buffGet(4202, 0) : nullptr;
    const auto time目标日斩 = buff目标日斩 && buff目标日斩->isValid ? buff目标日斩->nLeftFrame * 1024 / 16 : 0;
    const auto time目标月斩 = buff目标月斩 && buff目标月斩->isValid ? buff目标月斩->nLeftFrame * 1024 / 16 : 0;
    const auto cd生死劫     = skillCooldownLeftTick(生死劫);
    const auto cd暗尘弥散   = skillCooldownLeftTick(暗尘弥散);
    const auto buff诛邪     = buffGet(9909, 0);
    const auto buff悬象     = buffGet(25716, 0);
    const auto buff崇光     = buffGet(28194, 1);
    const auto buff斩恶     = buffGet(28195, 1);
    const auto buff连击     = buffGet(28196, 1);
    const auto stacknum崇光 = buff崇光 && buff崇光->isValid ? buff崇光->nStackNum : 0;
    const auto stacknum连击 = buff连击 && buff连击->isValid ? buff连击->nStackNum : 0;
    const auto buff同辉     = buffGet(12850, 2);
    const auto buff灵魂     = buffGet(25765, 1);
    const auto count灵魂 =
        (buff同辉 && buff同辉->isValid ? buff同辉->nStackNum * 5 : 0) +
        (buff灵魂 && buff灵魂->isValid ? buff灵魂->nStackNum : 0);
    const auto buff橙武CD = buffGet(2584, 0);
    const auto time橙武CD = buff橙武CD && buff橙武CD->isValid ? buff橙武CD->nLeftFrame * 1024 / 16 : 0;

    // 起手双斩
    if (now < 3 * 1024) {
        fightStopWait.emplace(1);
        if (time目标日斩 == 0)
            if (cast(烈日斩))
                return;
        if (time目标月斩 == 0)
            if (cast(银月斩))
                return;
    }

    // 调试
    if (now > 2 * fightTick)
        fightStopWait.emplace(0);

    // 处理日月同辉溢出
    if (count灵魂 >= 18 && (nSunPowerValue || nMoonPowerValue))
        for (int i = 0; i < count灵魂 - 17; i++)
            buffDel(25765, 1);

    // 连击崇光
    if (stacknum连击 > 0 && stacknum连击 < 3) {
        if (cast(崇光斩恶)) {
            if (stacknum连击 == 2 && fightStopWait.value() == 2)
                fightStopWait.emplace(0);
            return;
        }
    }

    // 溢出崇光
    if (stacknum崇光 >= 5 && buff斩恶 && buff斩恶->isValid) {
        if (buff悬象 && buff悬象->isValid) // 悬内点掉半崇
            buffDel(28195, 1);
        else if (time目标日斩 < 3 * 1024) { // 悬外补斩
            if (cast(烈日斩))
                return;
        } else if (time目标月斩 < 3 * 1024) { // 悬外补斩
            if (cast(银月斩))
                return;
        } else if (cast(崇光斩恶)) // 悬外崇
            return;
    }

    // 橙武特效
    if (time橙武CD > 25 * 1024)
        fightCW();

    // 隐前技能计数
    int count悬前 = 3; // 驱悬劫
    if (nCurrentSunEnergy < 4000)
        count悬前++;
    if (nCurrentMoonEnergy < 4000)
        count悬前++;
    if (stacknum崇光 >= 3)
        count悬前 += 3;
    if (buff诛邪 && buff诛邪->isValid)
        count悬前++;

    if (cd暗尘弥散 < 1024 * count悬前 - 512 && !(buff悬象 && buff悬象->isValid)) {
        if (stacknum崇光 >= 3) { // 悬前有三层崇光
            if (cast(崇光斩恶))
                return;
        }
        if (cd暗尘弥散 < 1024 * 2 - 512 && nCurrentSunEnergy >= 10000 && nCurrentMoonEnergy >= 10000)
            return fightBurst();
        if (count灵魂 >= 5)
            return fightConvVar();
        else
            return framePublicCooldown <= 14 ? fightConvVar() : fightConv();
    } else if (buff悬象 && buff悬象->isValid)
        return fightBurst();
    else {
        if (stacknum崇光 >= 3 && stacknum崇光 < 5 && // 崇光层数在3到5之间
            time目标日斩 > 3 * 1024 &&               // 目标日斩buff时长大于3秒, 并且
            now - fightTick > -20 * 1024             // 距离预定的收尾时间不到20s, 或已经超过了预定的收尾时间
        )
            if (cast(崇光斩恶)) {
                fightStopWait.emplace(2);
                return;
            }
        if (stacknum崇光 >= 5 &&     // 有5层崇光, 并且
            count灵魂 >= 8 &&        // 有8层灵魂, 并且
            time目标日斩 > 16 * 1024 // 目标日斩buff时长大于16秒
        )
            if (cast(崇光斩恶))
                return;
        if (stacknum崇光 >= 3 && cd生死劫 > 13 * 1024 - delayBase - delayRand) // 有3层崇光, 并且刚打完生死劫
            if (cast(崇光斩恶))
                return;
        return framePublicCooldown <= 14 ? fightConvVar() : fightConv();
    }
}

extern const std::vector<int> outset齐光一键;

void MjFysj::fight_简单_齐光一键() {
    if (bFight) {
        cast(outset齐光一键[nFight]);
        nFight++;
        if (nFight == outset齐光一键.size()) {
            bFight = false;
        }
    } else [[likely]] {
        const auto buff降灵尊 = buffGet(25731, 1);
        cast(3974); // 暗尘弥散
        if (buff降灵尊 && buff降灵尊->isValid) {
            itemUse(frame::ItemType::Trinket, 38789); // 吹香雪
            itemUse(frame::ItemType::Trinket, 39853); // 梧桐影
            cast(悬象著明);
        }
        cast(诛邪镇魔);
        cast(光明相);
        cast(生死劫);
        cast(净世破魔击);
        cast(驱夜断愁);
        cast(烈日斩);
        cast(银月斩);
        cast(赤日轮);
    }
}

extern const std::vector<int> outset崇光两键;

void MjFysj::fight_简单_崇光两键() {
    if (bFight) {
        cast(outset崇光两键[nFight]);
        nFight++;
        if (nFight == outset崇光两键.size()) {
            bFight = false;
        }
    } else [[likely]] {
        const auto buff崇光 = buffGet(28194, 1);
        const auto buff斩恶 = buffGet(28195, 1);
        const auto buff连击 = buffGet(28196, 1);
        if (buff连击 && buff连击->isValid && buff连击->nStackNum < 3)
            cast(37335); // 崇光斩恶, 连击
        if (buff崇光 && buff崇光->isValid && buff崇光->nStackNum == 5 && buff斩恶 && buff斩恶->isValid)
            cast(37335); // 崇光斩恶, 5.5 层
        if (nCurrentMoonEnergy <= 2000)
            cast(暗尘弥散);
        const auto buff降灵尊 = buffGet(25731, 1);
        if (buff降灵尊 && buff降灵尊->isValid) {
            itemUse(frame::ItemType::Trinket, 38789); // 吹香雪
            itemUse(frame::ItemType::Trinket, 39853); // 梧桐影
            cast(悬象著明);
        }
        cast(诛邪镇魔);
        cast(生死劫);
        cast(净世破魔击);
        if (!(buff降灵尊 && buff降灵尊->isValid) && buff崇光 && buff崇光->isValid && buff崇光->nStackNum >= 3)
            cast(37335); // 崇光斩恶, 3 层
        if (nCurrentMoonEnergy < 6000)
            cast(驱夜断愁);
        cast(烈日斩);
        cast(银月斩);
        cast(幽月轮);
    }
}

void MjFysj::fight_简单_崇光洞若() {
    const auto buff崇光     = buffGet(28194, 1);
    const auto buff斩恶     = buffGet(28195, 1);
    const auto buff连击     = buffGet(28196, 1);
    const auto stacknum崇光 = buff崇光 && buff崇光->isValid ? buff崇光->nStackNum : 0;
    const auto stacknum连击 = buff连击 && buff连击->isValid ? buff连击->nStackNum : 0;
    if (stacknum连击 > 0 && stacknum连击 < 3)
        cast(崇光斩恶);
    if (stacknum崇光 >= 5 && buff斩恶 && buff斩恶->isValid) {
        itemUse(frame::ItemType::Trinket, 38789); // 吹香雪
        itemUse(frame::ItemType::Trinket, 39853); // 梧桐影
        cast(崇光斩恶);
    }
    if (stacknum崇光 >= 3)
        cast(崇光斩恶);
    cast(暗尘弥散);
    if (buffExist(25731, 1))
        cast(悬象著明);
    cast(生死劫);
    cast(净世破魔击);
    if (nCurrentSunEnergy <= 8000 && nCurrentMoonEnergy <= 6000)
        cast(驱夜断愁);
    cast(烈日斩);
    cast(银月斩);
    cast(赤日轮);
}

const std::vector<int> queue崇光69 {
    日斩, 月斩, 月悬, 延迟隐身, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪, 日斩,
    驱夜, 日破, 月斩, 月劫, 崇光, 崇光, 崇光, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 诛邪, 日斩,
    驱夜, 日破, 月斩, 月劫, 崇光, 崇光, 崇光, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 诛邪, 日斩,
    月斩, 驱夜, 月悬, 延迟隐身, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 特效腰坠, 日破, 诛邪, 月破, 诛邪, 日斩, 崇光, 崇光, 崇光,
    驱夜, 日破, 月斩, 月劫, 崇光, 崇光, 崇光, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 诛邪, 日斩,
    驱夜, 日破, 月斩, 月劫, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 崇光, 崇光, 崇光, 诛邪, 日斩,
    月斩, 驱夜, 月悬, 延迟隐身, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪, 日斩,
    驱夜, 日破, 月斩, 月劫, 崇光, 崇光, 崇光, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 诛邪, 日斩,
    驱夜, 日破, 月斩, 月劫, 崇光, 崇光, 崇光, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 诛邪, 日斩,
    月斩, 驱夜, 月悬, 延迟隐身, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 特效腰坠, 日破, 诛邪, 月破, 诛邪, 日斩, 崇光, 崇光, 崇光,
    驱夜, 日破, 月斩, 月劫, 崇光, 崇光, 崇光, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 诛邪, 日斩,
    驱夜, 日破, 月斩, 月劫, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 崇光, 崇光, 崇光, 诛邪, 日斩,
    月斩, 驱夜, 月悬, 延迟隐身, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪, 日斩,
    驱夜, 日破, 月斩, 月劫, 崇光, 崇光, 崇光, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 诛邪, 日斩,
    驱夜, 日破, 月斩, 月劫, 崇光, 崇光, 崇光, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 诛邪, 日斩,
    月斩, 驱夜, 月悬, 延迟隐身, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 特效腰坠, 日破, 诛邪, 月破, 诛邪, 日斩, 崇光, 崇光, 崇光,
    驱夜, 日破, 月斩, 月劫, 崇光, 崇光, 崇光, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 诛邪, 日斩,
    驱夜, 日破, 月斩, 月劫, 诛邪, 日斩, 驱夜, 日破, 月斩, 月破, 崇光, 崇光, 崇光, 诛邪, 日斩,
    月斩, 驱夜, 月悬, 延迟隐身, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪, 日斩, 崇光, 崇光, 崇光
};

const std::vector<int> queue崇光699 {
    日斩, 月斩, 月悬, 延迟隐身, 日劫, 隐身,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 月斩, 崇光, 崇光, 崇光, 诛邪, 月斩, 月破,
    日斩, 驱夜, 日破, 诛邪, 月斩, 月破, 日斩, 驱夜, 日劫, 诛邪, 月轮, 月破,
    日斩, 驱夜, 日破, 点诛, 月斩, 月破, 崇光, 崇光, 崇光,
    日斩, 月斩, 驱夜, 月悬, 延迟隐身, 日劫, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 特效腰坠, 日破, 诛邪, 月破, 诛邪,
    月斩, 日斩, 崇光, 崇光, 崇光, 驱夜, 月破, 日劫, 月斩, 崇光, 崇光, 崇光, 诛邪, 月斩, 月破,
    日斩, 驱夜, 日破, 诛邪, 月斩, 月破, 日斩, 驱夜, 日劫, 诛邪, 月轮, 月破,
    日斩, 驱夜, 日破, 点诛, 月斩, 月破, 崇光, 崇光, 崇光,
    日斩, 月斩, 驱夜, 月悬, 延迟隐身, 日劫, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 月斩, 崇光, 崇光, 崇光, 点诛, 月斩, 月破, 崇光, 崇光, 崇光,
    日斩, 驱夜, 日破, 点诛, 月斩, 月破, 日斩, 驱夜, 日劫, 诛邪, 月轮, 月破,
    日斩, 驱夜, 日破, 点诛, 月斩, 月破, 崇光, 崇光, 崇光,
    日斩, 月斩, 驱夜, 月悬, 延迟隐身, 日劫, 隐身, 点诛,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 月斩, 崇光, 崇光, 崇光, 诛邪, 月斩, 月破,
    日斩, 驱夜, 日破, 诛邪, 月斩, 月破, 日斩, 驱夜, 日劫, 诛邪, 月轮, 月破,
    日斩, 驱夜, 日破, 点诛, 月斩, 月破, 崇光, 崇光, 崇光,
    日斩, 月斩, 驱夜, 月悬, 延迟隐身, 日劫, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    月斩, 日斩, 崇光, 崇光, 崇光, 驱夜, 月破, 日劫, 月斩, 崇光, 崇光, 崇光, 诛邪, 月斩, 月破,
    日斩, 驱夜, 日破, 诛邪, 月斩, 月破, 日斩, 驱夜, 日劫, 诛邪, 月轮, 月破,
    日斩, 驱夜, 日破, 点诛, 月斩, 月破, 崇光, 崇光, 崇光,
    日斩, 月斩, 驱夜, 月悬, 延迟隐身, 日劫, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 特效腰坠, 月破, 诛邪,
    月斩, 日斩, 驱夜, 月破, 日劫, 月斩, 崇光, 崇光, 崇光, 点诛, 月斩, 月破, 崇光, 崇光, 崇光,
    日斩, 驱夜, 日破, 点诛, 月斩, 月破, 日斩, 驱夜, 日劫, 诛邪, 月轮, 月破,
    日斩, 驱夜, 日破, 点诛, 月斩, 月破, 崇光, 崇光, 崇光,
    日斩, 月斩, 驱夜, 月悬, 延迟隐身, 日劫, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    日斩, 崇光, 崇光, 崇光
};

const std::vector<int> queue齐光30 {
    月斩, 月破, 日破, 日斩,
    隐身, 特效腰坠, 日悬, 月劫, 诛邪, 驱夜, 月斩, 月破, 诛邪, 日破, 诛邪,
    驱夜, 月破, 诛邪, 月斩, 日斩, 日破, 月斩, 光明相, 月劫, 日破, 诛邪,
    驱夜, 日斩, 日破, 月斩, 月破, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月轮, 月劫, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月斩, 特殊处理1, 月破, 日斩,
    隐身, 日悬, 月劫, 诛邪, 驱夜, 月斩, 月破, 诛邪, 日破, 诛邪,
    驱夜, 月破, 诛邪, 月斩, 日轮, 日破, 月斩, 月劫, 诛邪,
    驱夜, 日斩, 日破, 月斩, 月破, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月轮, 月劫, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月斩, 特殊处理1, 月破, 日斩,
    隐身, 特效腰坠, 日悬, 月劫, 诛邪, 驱夜, 月斩, 月破, 诛邪, 日破, 诛邪,
    驱夜, 月破, 诛邪, 月斩, 日斩, 日破, 月斩, 光明相, 月劫, 日破, 诛邪,
    驱夜, 日斩, 日破, 月斩, 月破, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月轮, 月劫, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月斩, 特殊处理1, 月破, 日斩,
    隐身, 日悬, 月劫, 诛邪, 驱夜, 月斩, 月破, 诛邪, 日破, 诛邪,
    驱夜, 月破, 诛邪, 月斩, 日轮, 日破, 月斩, 月劫, 诛邪,
    驱夜, 日斩, 日破, 月斩, 月破, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月轮, 月劫, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月斩, 特殊处理1, 月破, 日斩,
    隐身, 特效腰坠, 日悬, 月劫, 诛邪, 驱夜, 月斩, 月破, 诛邪, 日破, 诛邪,
    驱夜, 月破, 诛邪, 月斩, 日斩, 日破, 月斩, 光明相, 月劫, 日破, 诛邪,
    驱夜, 日斩, 日破, 月斩, 月破, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月轮, 月劫, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月斩, 特殊处理1, 月破, 日斩,
    隐身, 日悬, 月劫, 诛邪, 驱夜, 月斩, 月破, 诛邪, 日破, 诛邪,
    驱夜, 月破, 诛邪, 月斩, 日轮, 日破, 月斩, 月劫, 诛邪,
    驱夜, 日斩, 日破, 月斩, 月破, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月轮, 月劫, 诛邪, 驱夜, 日斩, 日破, 月斩, 月破, 诛邪,
    驱夜, 日斩, 日破, 月斩, 特殊处理1, 月破, 日斩,
    隐身, 特效腰坠, 日悬, 月劫, 诛邪, 驱夜, 月斩, 月破, 诛邪, 日破, 诛邪,
    驱夜, 月破, 诛邪, 月斩, 日斩, 日破
};

const std::vector<int> queue齐光35 {
    日斩, 月破, 日破, 驱夜, 月斩, 光明相, 月悬, 延迟隐身, 日劫, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    日斩, 月斩, 驱夜, 月破, 日劫, 诛邪, 日斩, 月斩, 驱夜, 月破, 日破, 诛邪,
    日斩, 月斩, 驱夜, 月破, 日破, 诛邪, 日斩, 月轮, 驱夜, 月破, 日劫,
    月斩, 诛邪, 月斩, 月破, 日斩, 驱夜, 日破, 诛邪, 月轮, 月破,
    日斩, 月斩, 驱夜, 月悬, 延迟隐身, 日劫, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    日斩, 驱夜, 日破, 月斩, 光明相, 月劫, 诛邪, 日破,
    日斩, 月斩, 驱夜, 月破, 诛邪, 日破, 日斩, 月斩, 驱夜, 月破, 诛邪, 日破,
    月斩, 驱夜, 月劫, 诛邪, 日斩, 日破, 日斩, 月斩, 驱夜, 月破, 诛邪, 日破,
    日斩, 月轮, 月斩, 驱夜, 月悬, 延迟隐身, 日劫, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    日斩, 月斩, 驱夜, 月破, 日劫, 诛邪, 日斩, 月斩, 驱夜, 月破, 日破, 诛邪,
    日斩, 月斩, 驱夜, 月破, 日破, 诛邪, 日斩, 月轮, 驱夜, 月破, 日劫,
    月斩, 诛邪, 月斩, 月破, 日斩, 驱夜, 日破, 诛邪, 月轮, 月破,
    日斩, 月斩, 驱夜, 月悬, 延迟隐身, 日劫, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    日斩, 驱夜, 日破, 月斩, 光明相, 月劫, 诛邪, 日破,
    日斩, 月斩, 驱夜, 月破, 诛邪, 日破, 日斩, 月斩, 驱夜, 月破, 诛邪, 日破,
    月斩, 驱夜, 月劫, 诛邪, 日斩, 日破, 日斩, 月斩, 驱夜, 月破, 诛邪, 日破,
    日斩, 月轮, 月斩, 驱夜, 月悬, 延迟隐身, 日劫, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    日斩, 月斩, 驱夜, 月破, 日劫, 诛邪, 日斩, 月斩, 驱夜, 月破, 日破, 诛邪,
    日斩, 月斩, 驱夜, 月破, 日破, 诛邪, 日斩, 月轮, 驱夜, 月破, 日劫,
    月斩, 诛邪, 月斩, 月破, 日斩, 驱夜, 日破, 诛邪, 月轮, 月破,
    日斩, 月斩, 驱夜, 月悬, 延迟隐身, 日劫, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪,
    日斩, 驱夜, 日破, 月斩, 光明相, 月劫, 诛邪, 日破,
    日斩, 月斩, 驱夜, 月破, 诛邪, 日破, 日斩, 月斩, 驱夜, 月破, 诛邪, 日破,
    月斩, 驱夜, 月劫, 诛邪, 日斩, 日破, 日斩, 月斩, 驱夜, 月破, 诛邪, 日破,
    日斩, 月轮, 月斩, 驱夜, 月悬, 延迟隐身, 日劫, 隐身, 诛邪,
    月破, 诛邪, 日破, 驱夜, 诛邪, 月破, 诛邪, 驱夜, 日破, 诛邪, 月破, 诛邪
};

const std::vector<int> outset齐光一键 {
    月斩, 月破, 日破, 日斩
};

const std::vector<int> outset崇光两键 {
    月破, 日破
};

} // namespace
