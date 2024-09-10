#pragma once

namespace jx3calc {
namespace frame {

/**
 * @brief 全局系数
 * @note 这是一个静态类.
 */
class GlobalParam {
public:
    static GlobalParam *instance() {
        static GlobalParam instance;
        return &instance;
    }
    static const GlobalParam &get() {
        return *instance();
    }
    double fPlayerCriticalCof                            = 0.75; // 裸体会效
    double fCriticalStrikeParam                          = 1.0;  // 会心
    double fCriticalStrikePowerParam                     = 1.0;  // 会效
    double fDefCriticalStrikeParam                       = 1.0;  // 御劲
    double fDecriticalStrikePowerParam                   = 1.0;  // 化劲
    double fHitValueParam                                = 1.0;  // 命中
    double fDodgeParam                                   = 1.0;  // 闪避
    double fParryParam                                   = 1.0;  // 招架
    double fInsightParam                                 = 1.0;  // 无双(识破)
    double fPhysicsShieldParam                           = 1.0;  // 外防
    double fMagicShieldParam                             = 1.0;  // 内防
    double fOvercomeParam                                = 1.0;  // 破防
    double fHasteRate                                    = 1.0;  // 急速
    double fToughnessDecirDamageCof                      = 1.0;  // 御劲效果
    double fSurplusParam                                 = 1.0;  // 破招伤害
    double fAssistedPowerCof                             = 1.0;  // 凝神
    double fDecriticalDamagePowerToPhysicsAttackPowerCof = 1.0;
    double fDecriticalDamagePowerToMagicAttackPowerCof   = 1.0;
    double fDecriticalDamagePowerToMaxLifeBaseCof        = 1.0;
    double nMaxDecriticalDamagePowerEableConvert         = 1.0;
    double fDecriticalDamagePVPCof                       = 1.0; // PVP 伤害系数

    static bool init();
    static int  levelCof(int level);

private:
    GlobalParam()                               = default;
    GlobalParam(const GlobalParam &)            = delete;
    GlobalParam &operator=(const GlobalParam &) = delete;
};

} // namespace frame
} // namespace jx3calc
