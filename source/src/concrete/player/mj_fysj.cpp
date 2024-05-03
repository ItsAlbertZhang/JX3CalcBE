#include "concrete/player.h"
#include <memory>

using namespace jx3calc;
using namespace concrete;

namespace {

const std::vector<std::tuple<int, int>> skills{
    {3962,  33}, // 赤日轮
    {3963,  32}, // 烈日斩
    {3966,  1 }, // 生死劫
    {3967,  32}, // 净世破魔击
    {3959,  24}, // 幽月轮
    {3960,  18}, // 银月斩
    {3969,  1 }, // 光明相
    {3974,  1 }, // 暗尘弥散
    {3979,  29}, // 驱夜断愁
    {22890, 1 }, // 诛邪镇魔
};

const std::vector<int> talents{
    5972,  // 腾焰飞芒
    18279, // 净身明礼
    22888, // 诛邪镇魔
    6717,  // 无明业火
    34383, // 明光恒照
    34395, // 日月同辉
    34372, // 靡业报劫
    17567, // 用晦而明
    25166, // 净体不畏
    34378, // 降灵尊
    34347, // 悬象著明
    37337, // 崇光斩恶
};

const std::vector<int> recipes{
    1005, // 赤日轮, 会心提高4%
    999,  // 赤日轮, 伤害提高3%
    1000, // 赤日轮, 伤害提高4%
    1001, // 赤日轮, 伤害提高5%
    1011, // 烈日斩, 会心提高4%
    1008, // 烈日斩, 伤害提高4%
    1009, // 烈日斩, 伤害提高5%
    1013, // 烈日斩, 对原地静止的目标伤害提升10%
    1621, // 生死劫, 伤害提高3%
    1622, // 生死劫, 伤害提高4%
    1623, // 生死劫, 伤害提高5%
    1019, // 净世破魔击, 会心提高5%
    1015, // 净世破魔击, 伤害提高4%
    1016, // 净世破魔击, 伤害提高5%
    5206, // 焚影圣诀心法下净世破魔击·月命中后回复20点月魂
    989,  // 幽月轮, 会心提高4%
    990,  // 幽月轮, 会心提高5%
    984,  // 幽月轮, 伤害提高3%
    985,  // 幽月轮, 伤害提高4%
    992,  // 银月斩, 会心提高3%
    993,  // 银月斩, 会心提高4%
    994,  // 银月斩, 会心提高5%
    1029, // 光明相, 调息时间减少10秒
    1030, // 光明相, 调息时间减少10秒
    1031, // 光明相, 调息时间减少10秒
    1055, // 驱夜断愁, 会心提高4%
    1056, // 驱夜断愁, 会心提高5%
    1052, // 驱夜断愁, 伤害提高4%
    1053, // 驱夜断愁, 伤害提高5%
};

namespace mj_fysj {

enum skill {
    日轮,
    月轮,
    日斩,
    月斩,
    日破,
    月破,
    日劫,
    月劫,
    光明相,
    隐身,
    驱夜,
    诛邪,
    日悬,
    月悬,
    崇光,
};

static const std::unordered_map<skill, int> skillMap{
    {日轮,    3962 },
    {月轮,    3959 },
    {日斩,    3963 },
    {月斩,    3960 },
    {日破,    3967 },
    {月破,    3967 },
    {日劫,    3966 },
    {月劫,    3966 },
    {光明相, 3969 },
    {隐身,    3974 },
    {驱夜,    3979 },
    {诛邪,    22890},
    {日悬,    34347},
    {月悬,    34347},
    {崇光,    37335},
};

// clang-format off

static const std::vector<skill> cg {
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

static const std::vector<skill> qg {
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

static const std::vector<skill> outset {
    月斩, 月破, 日破, 日斩,
};

// clang-format on

} // namespace mj_fysj

class MjFysj : public frame::Player {
public:
    MjFysj()
        : Player(10242, 13, &skills, &talents, &recipes, 503) {}

private:
    // bool macroSwitchedOnce = false;
    // bool highPing          = false;

    // // 宏0, 用于起手和进入大齐光的准备.
    // void macroDefault0() {
    //     if (nCurrentMoonEnergy >= 10000 || nCurrentMoonEnergy <= 2000)
    //         cast(3967); // [moon>99&sun<21] 净世破魔击
    //     cast(3979);     // 驱夜断愁
    //     cast(3963);     // 烈日斩
    //     if (nCurrentSunEnergy >= 10000 && nCurrentMoonEnergy == 8000) {
    //         // 切换至 1 号宏
    //         macroIdx = 1; // switch [sun>99&moon=80] 1
    //         if (macroSwitchedOnce == false) {
    //             // 起手时, 早一些释放暗尘弥散, 以避免第二轮卡隐身 CD
    //             delayCustom       = 300; // 将下一次释放宏的时机设置至 300 tick 后. 1024 tick 为 1 秒.
    //             // 如果不设置 player.delayCustom, 那么下一次释放宏的时机会是 公共 CD 冷却完成后 + 基础延迟 + 随机延迟
    //             macroSwitchedOnce = true; // 将起手标记置为 false
    //         }
    //     }
    // }

    // // 宏1, 用于 齐光3 前半部分.
    // void macroDefault1() {
    //     cast(3974); // 暗尘弥散
    //     // 如果隐身没好, 等隐身
    //     if (!buffExist(25731, 1) && !buffExist(25721, 3)) {
    //         // 在本宏的全部过程中, 要么有 降灵尊 buff (25731,1), 要么有 齐光3 buff (25721,3). 如果两者都没有, 说明隐身没有成功释放.
    //         return; // 直接返回, 不进行后续操作. 由于公共 CD 没有被触发, 因此下一次释放宏的时机为 基础延迟 + 随机延迟 后.
    //     }
    //     if (buffExist(25721, 3) && !buffExist(25716, 0) && nCurrentMoonEnergy >= 10000)
    //         cast(3969); // [buff:齐光3 & nobuff:悬象(包括日和月) & moon>99] 光明相
    //     cast(34347);    // 悬象著明
    //     if (buffExist(25716, 0)) {
    //         itemUse(frame::ItemType::Trinket, 38789); // [buff:悬象(包括日和月)] 吹香雪(特效腰坠)
    //     }
    //     cast(3966); // 生死劫
    //     // 如果生死劫没好, 等生死劫
    //     if (buffExist(25721, 1)) {
    //         // 生死劫会将齐光升至 2 阶. 如果此时仍有 齐光1 的 buff, 说明生死劫被卡 CD.
    //         return; // 直接返回, 不进行后续操作. 由于公共 CD 没有被触发, 因此下一次释放宏的时机为 基础延迟 + 随机延迟 后.
    //     }
    //     if (nCurrentMoonEnergy <= 4000) {
    //         cast(22890); // [moon<41] 诛邪镇魔
    //     }
    //     cast(3967); // 净世破魔击
    //     if (nCurrentMoonEnergy <= 4000) {
    //         cast(3979); // [moon<41] 驱夜断愁
    //     }
    //     cast(3960); // 银月斩
    //     cast(3963); // 烈日斩
    //     if (buffExist(25721, 3) && (!buffExist(25716, 0)) && nCurrentMoonEnergy == 0) {
    //         // 切换至 2 号宏
    //         macroIdx = 2; // switch [buff:齐光3 & nobuff:悬象(包括日和月) & moon=0] 2
    //     }
    // }

    // // 宏2, 用于 齐光3 后半部分.
    // void macroDefault2() {
    //     if (nCurrentSunEnergy >= 10000 && nCurrentMoonEnergy >= 10000) {
    //         // sun>=100&moon>=100, 说明 齐光3 结束返灵了. 走 齐光3 结束的流程.
    //         if (highPing) {
    //             // 处理高延迟情况.
    //             // 高延迟下, 即使 齐光3 结束返灵, 该日斩也必须打出, 否则会在小齐光部分开头卡月斩 CD.
    //             cast(3963); // 烈日斩
    //             if (nCurrentSunEnergy < 14000) {
    //                 // 日斩打出 + 齐光3 结束返还 100 能量, nCurrentSunEnergy < 14000 说明日斩未成功释放.
    //                 return; // 直接返回, 不进行后续操作. 由于公共 CD 没有被触发, 因此下一次释放宏的时机为 基础延迟 + 随机延迟 后.
    //             }
    //         }
    //         // 如果没有返回, 说明日斩成功释放, 可以继续走 齐光3 结束的流程.
    //         highPing = false; // 将高延迟标记重置为 false
    //         macroIdx = 3;     // 切换至 3 号宏
    //         macroDefault3();  // 执行一次 3 号宏
    //         return;           // 直接返回, 不进行后续操作
    //         // 在宏开始时进行判断的原因是, 导致切换条件 (sun>=100&moon>=100) 的事件 (齐光3 结束返灵) 是发生在公共 CD 中的,
    //         // 而不是发生在宏内由宏内的技能导致的. 因此, 必须在宏开始时进行判断, 而不能在宏结束时进行判断.
    //     }
    //     if (buffTimeLeftTick(25721, 3) < 5 * (1024 * 15 / 16 + delayBase + delayRand) && buffExist(9909, 0)) {
    //         // 齐光剩余时间小于 5 个 GCD, 且手上的诛邪还没打出去, 换高延迟打法
    //         highPing = true; // 将高延迟标记置为 true
    //     }

    //     // 高延迟打法
    //     if (highPing) {
    //         // 当 highPing 被标记时, 一定已经到了最后 5 个技能. 所以这里可以省去判定条件.
    //         cast(3963);  // 烈日斩
    //         cast(22890); // 诛邪镇魔
    //         cast(3960);  // 银月斩
    //         cast(3967);  // 净世破魔击
    //     }

    //     // 正常打法
    //     if (nCurrentMoonEnergy <= 4000) {
    //         cast(22890); // [moon<41] 诛邪镇魔
    //     }
    //     cast(3967); // 净世破魔击
    //     if (nCurrentMoonEnergy <= 4000) {
    //         cast(3979); // [moon<41] 驱夜断愁
    //     }
    //     cast(3963); // 烈日斩
    //     cast(3960); // 烈日斩
    // }

    // // 宏3, 用于 齐光3 结束后直至 齐光2 的结束阶段.
    // void macroDefault3() {
    //     if (nCurrentMoonEnergy >= 6000)
    //         cast(22890); // [moon>59] 诛邪镇魔
    //     if (nCurrentSunEnergy >= 10000)
    //         cast(3966); // [sun>99] 生死劫
    //     cast(3967);     // 净世破魔击
    //     cast(3960);     // 银月斩
    //     if (nCurrentMoonEnergy >= 6000 || nCurrentSunEnergy >= 6000)
    //         cast(3963); // [moon>59&sun>59] 烈日斩
    //     if (nCurrentMoonEnergy == 6000 && nCurrentSunEnergy == 4000)
    //         cast(3962); // [moon=60&sun=40] 赤日轮
    //     if (nCurrentMoonEnergy >= 10000 && nCurrentSunEnergy < 10000 && !buffExist(25721, 0)) {
    //         // 切换至 0 号宏
    //         macroIdx = 0; // switch [moon>99&sun<100&nobuff:齐光] 0
    //         // 在结尾进行判断的原因是, 导致切换条件 (moon>99&sun<100&nobuff:齐光) 的事件 (银月斩的释放) 是发生在宏内的. 因此, 可以在宏结束时进行判断.
    //     }
    // }

    int  idx        = 0;
    bool flagOutset = true;

    void embed0() {
        using namespace mj_fysj;
        if (skillGetLevel(37337)) { // 崇光
            if (idx == 0) [[unlikely]]
                stopInitiative.emplace(false);
            if (idx % 99 == 61) {                         // 特效腰坠
                itemUse(frame::ItemType::Trinket, 38789); // 吹香雪
                itemUse(frame::ItemType::Trinket, 39853); // 梧桐影
            }

            // 劫后 400ms 隐身, 多 1 降
            if (idx + 1 < cg.size() && cg[idx + 1] == skill::隐身)
                delayCustom = 400;

            if (cast(skillMap.at(cg[idx]))) {
                idx++;
            } else {
                auto tick = skillCooldownLeftTick(skillMap.at(cg[idx]));
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
            if (idx + 2 < qg.size() && qg[idx + 2] == mj_fysj::隐身) {
                auto tick = skillCooldownLeftTick(skillMap.at(qg[idx + 1])); // 获取日斩的剩余冷却时间
                auto t    = 1024 + delayBase + delayRand;
                if (tick > t) { // 假如剩余冷却时间大于 1 秒以上
                    delayCustom = tick - t;
                    return; // 直接返回, 不进行后续操作
                }
            }

            if (cast(skillMap.at(qg[idx]))) {
                idx++;
            } else {
                auto tick = skillCooldownLeftTick(skillMap.at(qg[idx]));
                if (tick > 0)
                    delayCustom = tick + delayBase + delayRand / 2;
                else
                    stopInitiative.reset();
            }
            if (idx == qg.size()) [[unlikely]]
                stopInitiative.emplace(true);

        } else [[unlikely]] {
            embedFightType = 1;
        }
    }

    void embed1() {
        if (flagOutset) {
            using namespace mj_fysj;
            cast(skillMap.at(outset[idx]));
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
    virtual int fightNormalAttack() override {
        skillCast(targetSelect, 4326, 1); // 大漠刀法
        int frame = 16 * 1024 / (1024 + this->chAttr.getHaste());
        return frame * 64; // 64 = 1024/16
    }
    virtual void fightDefault() override {
        if (embedFightType == 0) {
            embed0();
        } else if (embedFightType == 1) {
            embed1();
        }
    }
};
} // namespace

template <>
auto concrete::create<player::Type::MjFysj>() -> std::unique_ptr<frame::Player> {
    return std::make_unique<MjFysj>();
}
