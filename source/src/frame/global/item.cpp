#include "frame/global/item.h"
#include "gdi.h"
#include "plugin/log.h"
#include <cstdlib>
#include <stdexcept>

using namespace jx3calc;
using namespace frame;

const Item &ItemManager::get(ItemType type, int ID) {
    if (!data[static_cast<size_t>(type)].contains(ID))
        // 若 Item ID 不存在, 则添加并返回
        return add(type, ID);
    else
        return data.at(static_cast<size_t>(type)).at(ID);
}

const Item &ItemManager::add(ItemType type, int ID) {
    std::lock_guard<std::mutex> lock(mutex); // 加锁
    // 可能有多个线程同时进入了 add 函数. 因此, 需要在加锁后再次判断 Item 是否存在.
    if (data.at(static_cast<size_t>(type)).contains(ID)) {
        // 若 Item ID 存在, 则直接返回
        return data.at(static_cast<size_t>(type)).at(ID); // 返回时, 会自动调用 lock 的析构函数, 从而释放锁
    }

    // 初始化 Item
    Item item;
    item.ID = ID;
    // 获取 tab
    gdi::select_t arg;
    arg.emplace_back();
    arg[0]["ID"] = std::to_string(ID);
    switch (type) {
    case ItemType::Armor:
        gdi::tabSelect(gdi::Tab::custom_armor, arg);
        break;
    case ItemType::Trinket:
        gdi::tabSelect(gdi::Tab::custom_trinket, arg);
        break;
    case ItemType::Weapon:
        gdi::tabSelect(gdi::Tab::custom_weapon, arg);
        break;
    default:
        throw std::range_error("Invalid ItemType");
    }
    if (arg.size() == 0) {
        CONSTEXPR_LOG_ERROR("ItemManager::add: Item ID {} 不存在.", ID);
        data[static_cast<size_t>(type)][ID] = std::move(item);
        return data.at(static_cast<size_t>(type)).at(ID);
    }
    item.tab        = std::move(arg[0]);
    // 初始化数据. std::stoi() 用于确定字段存在的情况. 若该字段可能为空, 必须使用 atoi().
    item.type       = type;
    item.SkillID    = atoi(item.tab["SkillID"].c_str());
    item.SkillLevel = atoi(item.tab["SkillLevel"].c_str());
    item.CoolDownID = atoi(item.tab["CoolDownID"].c_str());

    // 将 Item 存入缓存
    data[static_cast<size_t>(type)][ID] = std::move(item);
    return data.at(static_cast<size_t>(type)).at(ID);
}
