#include "frame/buff.h"
#include "gdi.h"

using namespace ns_frame;

// 静态成员变量初始化
std::mutex BuffManager::mutex;
std::unordered_map<int, std::unordered_map<int, Buff>> BuffManager::data;

Buff &BuffManager::get(int buffID, int buffLevel) {
    // 若 Buff ID 和 Level 任一不存在, 则添加
    if (data.find(buffID) == data.end() || data[buffID].find(buffLevel) == data[buffID].end()) {
        add(buffID, buffLevel);
    }
    return data[buffID][buffLevel];
}

void BuffManager::add(int buffID, int buffLevel) {
    std::lock_guard<std::mutex> lock(mutex); // 加锁
    // 可能有多个线程同时进入了 add 函数. 因此, 需要在加锁后再次判断 Buff 是否存在.
    if (data.find(buffID) != data.end() && data[buffID].find(buffLevel) != data[buffID].end()) {
        // 若 Buff ID 存在, 且 Buff 等级存在, 则直接返回
        return; // 返回时, 会自动调用 lock 的析构函数, 从而释放锁
    }
    // 初始化 Buff
    Buff buff;
    // 获取 tab
    gdi::TabSelectType arg;
    arg.emplace_back();
    arg[0]["ID"] = std::to_string(buffID);
    arg[0]["Level"] = std::to_string(buffLevel);
    gdi::Interface::tabSelect(gdi::Tab::buff, arg);
    buff.tab = std::move(arg[0]);
    // TODO: 对 buff 进行处理
    // 将 Buff 存入缓存
    data[buffID][buffLevel] = std::move(buff);
}