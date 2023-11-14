#include "frame/global/buff.h"
#include "gdi.h"
#include "plugin/log.h"

using namespace jx3calc;
using namespace frame;

const Buff &BuffManager::get(int buffID, int buffLevel) {
    if (!data.contains(std::make_tuple(buffID, buffLevel)))
        // 若 Buff 不存在, 则添加并返回
        return add(buffID, buffLevel);
    else
        return data[std::make_tuple(buffID, buffLevel)];
}

static inline void addAttribute(std::vector<frame::Buff::Attrib> &attrib, ref::Attrib type, const std::string &valueA, const std::string &valueB) {
    auto &it     = attrib.emplace_back(type, valueA, valueB);
    // 尝试将 valueA 和 value B 转换为数字形态. 若转换失败, 则其会被置为 0, 且不会引发报错.
    it.valueAInt = atoi(valueA.c_str());
    it.valueBInt = atoi(valueB.c_str());
}
const Buff &BuffManager::add(int buffID, int buffLevel) {
    std::lock_guard<std::mutex> lock(mutex); // 加锁
    std::tuple<int, int>        key = std::make_tuple(buffID, buffLevel);
    // 可能有多个线程同时进入了 add 函数. 因此, 需要在加锁后再次判断 Buff 是否存在.
    if (data.contains(key)) {
        // 若 Buff ID 存在, 且 Buff 等级存在, 则直接返回
        return data.at(key); // 返回时, 会自动调用 lock 的析构函数, 从而释放锁
    }

    // 初始化 buff
    Buff buff;
    buff.ID    = buffID;
    buff.Level = buffLevel;
    // 获取 tab
    gdi::select_t arg;
    arg.emplace_back();
    arg[0]["ID"]    = std::to_string(buffID);
    arg[0]["Level"] = std::to_string(buffLevel);
    gdi::tabSelect(gdi::Tab::buff, arg);
    if (arg.size() == 0) {
        CONSTEXPR_LOG_ERROR("BuffManager::add: Buff ID {} 不存在.", buffID);
        data[key] = std::move(buff);
        return data.at(key);
    }
    buff.tab         = std::move(arg[0]);
    // 初始化数据. std::stoi() 用于确定字段存在的情况. 若该字段可能为空, 必须使用 atoi().
    buff.IsStackable = buff.tab["IsStackable"] == "1";
    buff.MaxStackNum = std::stoi(buff.tab["MaxStackNum"]);
    buff.Count       = std::stoi(buff.tab["Count"]);
    buff.Interval    = std::stoi(buff.tab["Interval"]);
    buff.Hide        = buff.tab["Hide"] == "1";
    buff.Exclude     = buff.tab["Exclude"] == "1";
    buff.ScriptFile  = buff.tab["ScriptFile"];
    buff.CanCancel   = buff.tab["CanCancel"] == "1";
    buff.MinInterval = std::stoi(buff.tab["MinInterval"]);
    buff.MaxInterval = std::stoi(buff.tab["MaxInterval"]);

    // 初始化 buff Attrib
    static const std::string attribName[] {"Begin", "Active", "EndTime"};
    for (int attribIdx = 0; attribIdx < 3; attribIdx++) {
        for (int i = 1; buff.tab.find(attribName[attribIdx] + "Attrib" + std::to_string(i)) != buff.tab.end(); i++) {

            std::string name = buff.tab[attribName[attribIdx] + "Attrib" + std::to_string(i)]; // BeginAttrib1, BeginAttrib2, ...
            if (name.empty())
                break; // 属性为空, 直接跳出, 进行下一个 attribIdx

            std::string valueA = buff.tab[attribName[attribIdx] + "Value" + std::to_string(i) + "A"]; // BeginValue1A, BeginValue2A, ...
            std::string valueB = buff.tab[attribName[attribIdx] + "Value" + std::to_string(i) + "B"]; // BeginValue1B, BeginValue2B, ...

            if (Ref<ref::Attrib>::map.find(name) == Ref<ref::Attrib>::map.end()) {
                CONSTEXPR_LOG_ERROR("BuffManager::add: {} Unknown Attribute: {}", attribIdx, name);
                // source/include/frame/ref/tab_attribute.h
                continue;
            }
            switch (attribIdx) {
            case 0:
                addAttribute(buff.BeginAttrib, Ref<ref::Attrib>::map.at(name), valueA, valueB);
                break;
            case 1:
                addAttribute(buff.ActiveAttrib, Ref<ref::Attrib>::map.at(name), valueA, valueB);
                break;
            case 2:
                addAttribute(buff.EndTimeAttrib, Ref<ref::Attrib>::map.at(name), valueA, valueB);
                break;
            }
        }
    }

    // 查询 Buff UI
    arg.clear();
    arg.emplace_back();
    arg[0]["BuffID"] = std::to_string(buffID);
    arg[0]["Level"]  = std::to_string(buffLevel);
    arg.emplace_back();
    arg[1]["BuffID"] = std::to_string(buffID);
    arg[1]["Level"]  = "0";
    gdi::tabSelect(gdi::Tab::ui_buff, arg);
    if (arg.size() == 0) {
        buff.Name = "未知技能";
    } else {
        buff.ui   = std::move(arg[0]);
        buff.Name = buff.ui["Name"];
    }

    // 将 Buff 存入缓存
    data[key] = std::move(buff);
    return data.at(key);
}
