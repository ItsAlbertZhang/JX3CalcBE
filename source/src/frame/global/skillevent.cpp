#include "frame/global/skillevent.h"
#include "gdi.h"
#include "plugin/log.h"

using namespace jx3calc;
using namespace frame;

const SkillEvent &SkillEventManager::get(int ID) {
    if (!data.contains(ID))
        // 若 SkillEvent ID 不存在, 则添加并返回
        return add(ID);
    else
        return data.at(ID);
}

const SkillEvent &SkillEventManager::add(int ID) {
    std::lock_guard<std::mutex> lock(mutex); // 加锁
    // 可能有多个线程同时进入了 add 函数. 因此, 需要在加锁后再次判断 SkillEvent 是否存在.
    if (data.contains(ID)) {
        // 若 SkillEvent ID 存在, 则直接返回
        return data.at(ID); // 返回时, 会自动调用 lock 的析构函数, 从而释放锁
    }

    // 初始化 SkillEvent
    SkillEvent skillevent;
    skillevent.ID = ID;
    // 获取 tab
    gdi::select_t arg;
    arg.emplace_back();
    arg[0]["ID"] = std::to_string(ID);
    gdi::tabSelect(gdi::Tab::skillevent, arg);
    if (arg.size() == 0) {
        CONSTEXPR_LOG_ERROR("SkillEventManager::add: SkillEvent ID {} 不存在.", ID);
        data[ID] = std::move(skillevent);
        return data.at(ID);
    }
    skillevent.tab          = std::move(arg[0]);
    // 初始化数据. std::stoi() 用于确定字段存在的情况. 若该字段可能为空, 必须使用 atoi().
    auto &m_et              = Ref<ref::SkillEvent::EventType>::map;
    auto &m_ct              = Ref<ref::SkillEvent::CasterTarget>::map;
    skillevent.type         = m_et.at(skillevent.tab["EventType"]);
    skillevent.Odds         = std::stoi(skillevent.tab["Odds"]);
    skillevent.SkillID      = std::stoi(skillevent.tab["SkillID"]);
    skillevent.SkillLevel   = std::stoi(skillevent.tab["SkillLevel"]);
    skillevent.SkillCaster  = m_ct.at(skillevent.tab["SkillCaster"]);
    skillevent.SkillTarget  = m_ct.at(skillevent.tab["SkillTarget"]);
    skillevent.EventMask1   = static_cast<uint32_t>(std::stoull(skillevent.tab["EventMask1"]));
    skillevent.EventMask2   = static_cast<uint32_t>(std::stoull(skillevent.tab["EventMask2"]));
    skillevent.EventSkillID = std::stoi(skillevent.tab["EventSkillID"]);

    // 将 SkillEvent 存入缓存
    data[ID] = std::move(skillevent);
    return data.at(ID);
}
