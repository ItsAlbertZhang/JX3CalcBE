#include "frame/global/cooldown.h"
#include "gdi.h"

using namespace ns_frame;

const Cooldown &CooldownManager::get(int ID) {
    // 若 Cooldown ID 不存在, 则添加
    if (data.find(ID) == data.end()) {
        add(ID);
    }
    return data[ID];
}

void CooldownManager::add(int ID) {
    std::lock_guard<std::mutex> lock(mutex); // 加锁
    // 可能有多个线程同时进入了 add 函数. 因此, 需要在加锁后再次判断 Cooldown 是否存在.
    if (data.find(ID) != data.end()) {
        // 若 Cooldown ID 存在, 则直接返回
        return; // 返回时, 会自动调用 lock 的析构函数, 从而释放锁
    }

    Cooldown cooldown;
    // 获取 tab
    gdi::TabSelectType arg;
    arg.emplace_back();
    arg[0]["ID"] = std::to_string(ID);
    gdi::Interface::tabSelect(gdi::Tab::cooldown, arg);
    cooldown.tab = std::move(arg[0]);
    // 初始化 Cooldown
    cooldown.ID = ID;
    cooldown.Duration = std::stoi(cooldown.tab["Duration"]);
    cooldown.MinDuration = std::stoi(cooldown.tab["MinDuration"]);
    cooldown.MaxDuration = std::stoi(cooldown.tab["MaxDuration"]);
    cooldown.MaxCount = std::stoi(cooldown.tab["MaxCount"]);
    // 将 Cooldown 存入缓存
    data[ID] = std::move(cooldown);
}