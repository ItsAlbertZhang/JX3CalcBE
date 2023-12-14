#include "concrete/character/player/mj_fysj.h"

using namespace ns_concrete;

MjFysj::MjFysj() {
    delayBase = 45; // 网络延迟, 单位为 tick
    delayRand = 20; // 按键延迟, 单位为 tick

    skillLearn(10242, 13); // 焚影圣诀
    dwKungfuID       = 10242;
    publicCooldownID = 503; // GCD
    skillActive(10242);     // 激活心法加成

    skillLearn(3962, 33); // 赤日轮
    skillLearn(3963, 32); // 烈日斩
    skillLearn(3966, 1);  // 生死劫
    skillLearn(3967, 32); // 净世破魔击
    skillLearn(3959, 24); // 幽月轮
    skillLearn(3960, 18); // 银月斩
    skillLearn(3969, 1);  // 光明相
    skillLearn(3974, 1);  // 暗尘弥散
    skillLearn(3979, 29); // 驱夜断愁

    skillLearn(5972, 1);  // 腾焰飞芒
    skillLearn(18279, 1); // 净身明礼
    skillLearn(22888, 1); // 诛邪镇魔
    skillLearn(22890, 1); // 诛邪镇魔, 主动
    skillLearn(6717, 1);  // 无明业火
    skillLearn(34383, 1); // 明光恒照
    skillLearn(34395, 1); // 日月同辉
    skillLearn(34372, 1); // 靡业报劫
    skillLearn(17567, 1); // 用晦而明
    skillLearn(25166, 1); // 净体不畏
    skillLearn(34378, 1); // 降灵尊
    skillLearn(34347, 1); // 悬象著明, 主动
    skillLearn(34370, 1); // 日月齐光

    skillActive(5972);
    skillActive(18279);
    skillActive(22888);
    // skillActive(22890); // 主动技能不需要激活
    skillActive(6717);
    skillActive(34383);
    skillActive(34395);
    skillActive(34372);
    skillActive(17567);
    skillActive(25166);
    skillActive(34378);
    // skillActive(34347); // 主动技能不需要激活
    skillActive(34370);

    skillrecipeAdd(1005, 1); // 赤日轮, 会心提高4%
    skillrecipeAdd(999, 1);  // 赤日轮, 伤害提高3%
    skillrecipeAdd(1000, 1); // 赤日轮, 伤害提高4%
    skillrecipeAdd(1001, 1); // 赤日轮, 伤害提高5%

    skillrecipeAdd(1011, 1); // 烈日斩, 会心提高4%
    skillrecipeAdd(1008, 1); // 烈日斩, 伤害提高4%
    skillrecipeAdd(1009, 1); // 烈日斩, 伤害提高5%
    skillrecipeAdd(1013, 1); // 烈日斩, 对原地静止的目标伤害提升10%

    skillrecipeAdd(1621, 1); // 生死劫, 伤害提高3%
    skillrecipeAdd(1622, 1); // 生死劫, 伤害提高4%
    skillrecipeAdd(1623, 1); // 生死劫, 伤害提高5%

    skillrecipeAdd(1019, 1); // 净世破魔击, 会心提高5%
    skillrecipeAdd(1015, 1); // 净世破魔击, 伤害提高4%
    skillrecipeAdd(1016, 1); // 净世破魔击, 伤害提高5%
    skillrecipeAdd(5206, 1); // 焚影圣诀心法下净世破魔击·月命中后回复20点月魂

    skillrecipeAdd(989, 1); // 幽月轮, 会心提高4%
    skillrecipeAdd(990, 1); // 幽月轮, 会心提高5%
    skillrecipeAdd(984, 1); // 幽月轮, 伤害提高3%
    skillrecipeAdd(985, 1); // 幽月轮, 伤害提高4%

    skillrecipeAdd(992, 1); // 银月斩, 会心提高3%
    skillrecipeAdd(993, 1); // 银月斩, 会心提高4%
    skillrecipeAdd(994, 1); // 银月斩, 会心提高5%

    skillrecipeAdd(1029, 1); // 光明相, 调息时间减少10秒
    skillrecipeAdd(1030, 1); // 光明相, 调息时间减少10秒
    skillrecipeAdd(1031, 1); // 光明相, 调息时间减少10秒

    skillrecipeAdd(1055, 1); // 驱夜断愁, 会心提高4%
    skillrecipeAdd(1056, 1); // 驱夜断愁, 会心提高5%
    skillrecipeAdd(1052, 1); // 驱夜断愁, 伤害提高4%
    skillrecipeAdd(1053, 1); // 驱夜断愁, 伤害提高5%
}

// virtual override
void MjFysj::macroPrepareDefault() {
    cast(3974);
    if (nSunPowerValue == 0 && nMoonPowerValue == 0) {
        if (nCurrentMoonEnergy >= 10000)
            nMoonPowerValue = 1;
        else if (nCurrentSunEnergy >= 10000)
            nSunPowerValue = 1;
    }
}

// virtual override
void MjFysj::macroRuntimeDefault() {
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
    }
}

void MjFysj::macroDefault0() {
    if (nCurrentMoonEnergy >= 10000 || nCurrentMoonEnergy <= 2000)
        cast(3967); // 净世破魔击
    cast(3979);     // 驱夜断愁
    cast(3963);     // 烈日斩
    if (nCurrentSunEnergy >= 10000 && nCurrentMoonEnergy == 8000) {
        macroIdx = 1; // 切换至 1 号宏
        if (macroSwitchedOnce == false) {
            // 起手时, 早一些释放暗尘弥散, 以避免第二轮卡隐身CD
            delayCustom = 300; // 将下一次释放宏的时机设置至 300 tick 后
            // 如果不进行设置, 那么下一次释放宏的时机会是 GCD 结束后 + 网络延迟 + 随机按键延迟.
            macroSwitchedOnce = true; // 将起手标记置为 false
        }
    }
}

void MjFysj::macroDefault1() {
    if (nCurrentSunEnergy >= 10000 && nCurrentMoonEnergy >= 10000) {
        macroIdx = 2;    // 切换至 2 号宏
        macroDefault2(); // 执行一次 2 号宏
        return;          // 直接返回, 不进行后续操作
        // 在开头进行判断的原因是, 导致切换条件 (sun>=100&moon>=100) 的事件 (日月齐光·叁结束) 是发生在 GCD 中的, 而不是发生在宏内的. 因此, 无法在宏结束时进行判断.
    }

    cast(3974); // 暗尘弥散
    if (buffExist(25721, 3) && !buffExist(25716, 0) && nCurrentMoonEnergy >= 10000)
        cast(3969); // 光明相
    cast(34347);    // 悬象著明
    cast(3966);     // 生死劫

    if (!buffExist(25721, 1)) { // 解决生死劫卡CD提前打破魔击的问题, 进行日月齐光·壹判定
        if (nCurrentMoonEnergy <= 4000) {
            cast(22890); // 诛邪镇魔
        }
        cast(3967); // 净世破魔击
        if (nCurrentMoonEnergy <= 4000) {
            cast(3979); // 驱夜断愁
        }
        cast(3960); // 银月斩
        cast(3963); // 烈日斩
    }
}

void MjFysj::macroDefault2() {
    if (nCurrentMoonEnergy >= 6000)
        cast(22890); // 诛邪镇魔
    if (nCurrentSunEnergy >= 10000)
        cast(3966); // 生死劫
    cast(3967);     // 净世破魔击
    cast(3960);     // 银月斩
    if (nCurrentMoonEnergy >= 6000 || nCurrentSunEnergy >= 6000)
        cast(3963); // 烈日斩
    if (nCurrentMoonEnergy == 6000 && nCurrentSunEnergy == 4000)
        cast(3962); // 赤日轮
    if (nCurrentMoonEnergy >= 10000 && nCurrentSunEnergy < 10000 && !buffExist(25721, 0)) {
        macroIdx = 0; // 切换至 0 号宏
        // 在结尾进行判断的原因时, 导致切换条件 (moon>=100 & sun<100 & nobuff:25721) 的事件 (银月斩的释放) 是发生在宏内的. 因此, 可以在宏结束时进行判断.
    }
}

// virtual override
int MjFysj::normalAttack() {
    skillCast(targetSelect, 4326, 1); // 大漠刀法
    int frame = 16 * (1024 - this->chAttr.getHaste()) / 1024;
    return frame * 64; // 64 = 1024/16
}
