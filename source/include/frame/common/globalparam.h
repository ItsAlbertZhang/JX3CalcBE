#pragma once

#include <mutex>

namespace jx3calc {
namespace frame {

/**
 * @brief 全局系数
 * @note 这是一个静态类.
 * @note 原本应该从 lua 中读取的, 但是懒得写接口了, 就直接写成静态的好了.
 * @note 未来需要从 lua 中读取的话, 可以考虑将 getVariable 压入 lua 中, 然后调用 lua 初始化即可.
 */
class GlobalParam {
public:
    static const GlobalParam &get() {
        return nullptr != ptr ? *ptr : getVariable();
    }
    static GlobalParam &getVariable() {
        std::lock_guard<std::mutex> lock(mutex); // 加锁
        if (ptr == nullptr) {
            ptr = new GlobalParam();
        }
        return *ptr;
    }
    double fPlayerCriticalCof          = 0.75;
    double fCriticalStrikeParam        = 9.530;
    double fCriticalStrikePowerParam   = 3.335;
    double fDefCriticalStrikeParam     = 9.530;
    double fDecriticalStrikePowerParam = 1.380;
    double fHitValueParam              = 6.931;
    double fDodgeParam                 = 3.703;
    double fParryParam                 = 4.345;
    double fInsightParam               = 9.189;
    double fPhysicsShieldParam         = 5.091;
    double fMagicShieldParam           = 5.091;
    double fOvercomeParam              = 9.530;
    double fHasteRate                  = 11.695;
    double fToughnessDecirDamageCof    = 2.557;
    double fSurplusParam               = 13.192;
    double fAssistedPowerCof           = 9.53;

    static int levelCof(int level) {
        if (level > 110)
            return 450 * (level - 110) + 3750;
        else if (level > 100)
            return 205 * (level - 100) + 1700;
        else if (level > 95)
            return 185 * (level - 95) + 775;
        else if (level > 90)
            return 85 * (level - 90) + 350;
        else if (level > 15)
            return 4 * level - 10;
        else
            return 50;
    }

private:
    GlobalParam()                  = default;
    static inline GlobalParam *ptr = nullptr;
    static inline std::mutex   mutex; // 互斥锁
};

} // namespace frame
} // namespace jx3calc
