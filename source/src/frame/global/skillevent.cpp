#include "frame/global/skillevent.h"
#include "gdi.h"

using namespace jx3calc;
using namespace frame;

const SkillEvent &SkillEventManager::get(int ID) {
    // 若 SkillEvent ID 不存在, 则添加
    if (data.find(ID) == data.end()) {
        add(ID);
    }
    return data[ID];
}

void SkillEventManager::add(int ID) {
    std::lock_guard<std::mutex> lock(mutex); // 加锁
    // 可能有多个线程同时进入了 add 函数. 因此, 需要在加锁后再次判断 SkillEvent 是否存在.
    if (data.find(ID) != data.end()) {
        // 若 SkillEvent ID 存在, 则直接返回
        return; // 返回时, 会自动调用 lock 的析构函数, 从而释放锁
    }

    // 初始化 SkillEvent
    SkillEvent skillevent;
    skillevent.ID = ID;
    // 获取 tab
    gdi::select_t arg;
    arg.emplace_back();
    arg[0]["ID"] = std::to_string(ID);
    gdi::tabSelect(gdi::Tab::skillevent, arg);
    skillevent.tab = std::move(arg[0]);

    using typeEventType    = Ref<ref::SkillEvent::EventType>::Type;
    using typeCasterTarget = Ref<ref::SkillEvent::CasterTarget>::Type;

    auto &mapEventType    = Ref<ref::SkillEvent::EventType>::map;
    auto &mapCasterTarget = Ref<ref::SkillEvent::CasterTarget>::map;

    // 初始化数据. std::stoi() 用于确定字段存在的情况. 若该字段可能为空, 必须使用 atoi().
    skillevent.type         = mapEventType.contains(skillevent.tab["EventType"]) ? mapEventType.at(skillevent.tab["EventType"]) : typeEventType::COUNT;
    skillevent.Odds         = std::stoi(skillevent.tab["Odds"]);
    skillevent.SkillID      = std::stoi(skillevent.tab["SkillID"]);
    skillevent.SkillLevel   = std::stoi(skillevent.tab["SkillLevel"]);
    skillevent.SkillCaster  = mapCasterTarget.contains(skillevent.tab["SkillCaster"]) ? mapCasterTarget.at(skillevent.tab["SkillCaster"]) : typeCasterTarget::COUNT;
    skillevent.SkillTarget  = mapCasterTarget.contains(skillevent.tab["SkillTarget"]) ? mapCasterTarget.at(skillevent.tab["SkillTarget"]) : typeCasterTarget::COUNT;
    skillevent.EventMask1   = std::stoi(skillevent.tab["EventMask1"]);
    skillevent.EventMask2   = std::stoi(skillevent.tab["EventMask2"]);
    skillevent.EventSkillID = std::stoi(skillevent.tab["EventSkillID"]);
    // 将 SkillEvent 存入缓存
    data[ID]                = std::move(skillevent);
}
