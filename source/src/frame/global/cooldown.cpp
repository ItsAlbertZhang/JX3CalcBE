#include "frame/global/cooldown.h"
#include "gdi.h"
#include "plugin/log.h"

using namespace jx3calc;
using namespace frame;

const Cooldown &CooldownManager::get(int ID) {
    if (!data.contains(ID))
        // 若 Cooldown ID 不存在, 则添加并返回
        return add(ID);
    else
        return data.at(ID);
}

const Cooldown &CooldownManager::add(int ID) {
    std::lock_guard<std::mutex> lock(mutex); // 加锁
    // 可能有多个线程同时进入了 add 函数. 因此, 需要在加锁后再次判断 Cooldown 是否存在.
    if (data.contains(ID)) {
        // 若 Cooldown ID 存在, 则直接返回
        return data.at(ID); // 返回时, 会自动调用 lock 的析构函数, 从而释放锁
    }

    // 初始化 Cooldown
    Cooldown cooldown;
    cooldown.ID = ID;
    // 获取 tab
    gdi::select_t arg;
    arg.emplace_back();
    arg[0]["ID"] = std::to_string(ID);
    gdi::tabSelect(gdi::Tab::cooldown, arg);
    if (arg.size() == 0) {
        CONSTEXPR_LOG_ERROR("CooldownManager::add: Cooldown ID {} 不存在.", ID);
        data[ID] = std::move(cooldown);
        return data.at(ID);
    }
    cooldown.tab              = std::move(arg[0]);
    // 初始化数据. std::stoi() 用于确定字段存在的情况. 若该字段可能为空, 必须使用 atoi().
    cooldown.DurationFrame    = static_cast<int>(std::stod(cooldown.tab["Duration"]) * 16 + 0.5);
    cooldown.MinDurationFrame = static_cast<int>(std::stod(cooldown.tab["MinDuration"]) * 16 + 0.5);
    cooldown.MaxDurationFrame = static_cast<int>(std::stod(cooldown.tab["MaxDuration"]) * 16 + 0.5);
    cooldown.MaxCount         = std::stoi(cooldown.tab["MaxCount"]);

    // 将 Cooldown 存入缓存
    data[ID] = std::move(cooldown);
    return data.at(ID);
}
