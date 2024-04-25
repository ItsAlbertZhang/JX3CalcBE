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
    34347, // 悬象著明 (主动)
    34370, // 日月齐光
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

class MjFysj : public frame::Player {
public:
    MjFysj()
        : Player(10242, 13, &skills, &talents, &recipes, 503) {}

private:
    bool macroSwitchedOnce = false;
    bool highPing          = false;

    // 宏0, 用于起手和进入大齐光的准备.
    void macroDefault0() {
        if (nCurrentMoonEnergy >= 10000 || nCurrentMoonEnergy <= 2000)
            cast(3967); // [moon>99&sun<21] 净世破魔击
        cast(3979);     // 驱夜断愁
        cast(3963);     // 烈日斩
        if (nCurrentSunEnergy >= 10000 && nCurrentMoonEnergy == 8000) {
            // 切换至 1 号宏
            macroIdx = 1; // switch [sun>99&moon=80] 1
            if (macroSwitchedOnce == false) {
                // 起手时, 早一些释放暗尘弥散, 以避免第二轮卡隐身 CD
                delayCustom       = 300; // 将下一次释放宏的时机设置至 300 tick 后. 1024 tick 为 1 秒.
                // 如果不设置 player.delayCustom, 那么下一次释放宏的时机会是 公共 CD 冷却完成后 + 基础延迟 + 随机延迟
                macroSwitchedOnce = true; // 将起手标记置为 false
            }
        }
    }

    // 宏1, 用于 齐光3 前半部分.
    void macroDefault1() {
        cast(3974); // 暗尘弥散
        // 如果隐身没好, 等隐身
        if (!buffExist(25731, 1) && !buffExist(25721, 3)) {
            // 在本宏的全部过程中, 要么有 降灵尊 buff (25731,1), 要么有 齐光3 buff (25721,3). 如果两者都没有, 说明隐身没有成功释放.
            return; // 直接返回, 不进行后续操作. 由于公共 CD 没有被触发, 因此下一次释放宏的时机为 基础延迟 + 随机延迟 后.
        }
        if (buffExist(25721, 3) && !buffExist(25716, 0) && nCurrentMoonEnergy >= 10000)
            cast(3969); // [buff:齐光3 & nobuff:悬象(包括日和月) & moon>99] 光明相
        cast(34347);    // 悬象著明
        if (buffExist(25716, 0)) {
            itemUse(frame::ItemType::Trinket, 38789); // [buff:悬象(包括日和月)] 吹香雪(特效腰坠)
        }
        cast(3966); // 生死劫
        // 如果生死劫没好, 等生死劫
        if (buffExist(25721, 1)) {
            // 生死劫会将齐光升至 2 阶. 如果此时仍有 齐光1 的 buff, 说明生死劫被卡 CD.
            return; // 直接返回, 不进行后续操作. 由于公共 CD 没有被触发, 因此下一次释放宏的时机为 基础延迟 + 随机延迟 后.
        }
        if (nCurrentMoonEnergy <= 4000) {
            cast(22890); // [moon<41] 诛邪镇魔
        }
        cast(3967); // 净世破魔击
        if (nCurrentMoonEnergy <= 4000) {
            cast(3979); // [moon<41] 驱夜断愁
        }
        cast(3960); // 银月斩
        cast(3963); // 烈日斩
        if (buffExist(25721, 3) && (!buffExist(25716, 0)) && nCurrentMoonEnergy == 0) {
            // 切换至 2 号宏
            macroIdx = 2; // switch [buff:齐光3 & nobuff:悬象(包括日和月) & moon=0] 2
        }
    }

    // 宏2, 用于 齐光3 后半部分.
    void macroDefault2() {
        if (nCurrentSunEnergy >= 10000 && nCurrentMoonEnergy >= 10000) {
            // sun>=100&moon>=100, 说明 齐光3 结束返灵了. 走 齐光3 结束的流程.
            if (highPing) {
                // 处理高延迟情况.
                // 高延迟下, 即使 齐光3 结束返灵, 该日斩也必须打出, 否则会在小齐光部分开头卡月斩 CD.
                cast(3963); // 烈日斩
                if (nCurrentSunEnergy < 14000) {
                    // 日斩打出 + 齐光3 结束返还 100 能量, nCurrentSunEnergy < 14000 说明日斩未成功释放.
                    return; // 直接返回, 不进行后续操作. 由于公共 CD 没有被触发, 因此下一次释放宏的时机为 基础延迟 + 随机延迟 后.
                }
            }
            // 如果没有返回, 说明日斩成功释放, 可以继续走 齐光3 结束的流程.
            highPing = false; // 将高延迟标记重置为 false
            macroIdx = 3;     // 切换至 3 号宏
            macroDefault3();  // 执行一次 3 号宏
            return;           // 直接返回, 不进行后续操作
            // 在宏开始时进行判断的原因是, 导致切换条件 (sun>=100&moon>=100) 的事件 (齐光3 结束返灵) 是发生在公共 CD 中的,
            // 而不是发生在宏内由宏内的技能导致的. 因此, 必须在宏开始时进行判断, 而不能在宏结束时进行判断.
        }
        if (buffTimeLeftTick(25721, 3) < 5 * (1024 * 15 / 16 + delayBase + delayRand) && buffExist(9909, 0)) {
            // 齐光剩余时间小于 5 个 GCD, 且手上的诛邪还没打出去, 换高延迟打法
            highPing = true; // 将高延迟标记置为 true
        }

        // 高延迟打法
        if (highPing) {
            // 当 highPing 被标记时, 一定已经到了最后 5 个技能. 所以这里可以省去判定条件.
            cast(3963);  // 烈日斩
            cast(22890); // 诛邪镇魔
            cast(3960);  // 银月斩
            cast(3967);  // 净世破魔击
        }

        // 正常打法
        if (nCurrentMoonEnergy <= 4000) {
            cast(22890); // [moon<41] 诛邪镇魔
        }
        cast(3967); // 净世破魔击
        if (nCurrentMoonEnergy <= 4000) {
            cast(3979); // [moon<41] 驱夜断愁
        }
        cast(3963); // 烈日斩
        cast(3960); // 烈日斩
    }

    // 宏3, 用于 齐光3 结束后直至 齐光2 的结束阶段.
    void macroDefault3() {
        if (nCurrentMoonEnergy >= 6000)
            cast(22890); // [moon>59] 诛邪镇魔
        if (nCurrentSunEnergy >= 10000)
            cast(3966); // [sun>99] 生死劫
        cast(3967);     // 净世破魔击
        cast(3960);     // 银月斩
        if (nCurrentMoonEnergy >= 6000 || nCurrentSunEnergy >= 6000)
            cast(3963); // [moon>59&sun>59] 烈日斩
        if (nCurrentMoonEnergy == 6000 && nCurrentSunEnergy == 4000)
            cast(3962); // [moon=60&sun=40] 赤日轮
        if (nCurrentMoonEnergy >= 10000 && nCurrentSunEnergy < 10000 && !buffExist(25721, 0)) {
            // 切换至 0 号宏
            macroIdx = 0; // switch [moon>99&sun<100&nobuff:齐光] 0
            // 在结尾进行判断的原因是, 导致切换条件 (moon>99&sun<100&nobuff:齐光) 的事件 (银月斩的释放) 是发生在宏内的. 因此, 可以在宏结束时进行判断.
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
        switch (macroIdx) {
        case 0:
            macroDefault0();
            break;
        case 1:
            macroDefault1();
            break;
        case 2:
            macroDefault2();
            break;
        case 3:
            macroDefault3();
            break;
        }
    }
};
} // namespace

template <>
auto concrete::create<player::Type::MjFysj>() -> std::unique_ptr<frame::Player> {
    return std::make_unique<MjFysj>();
}
