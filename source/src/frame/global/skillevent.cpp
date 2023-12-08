#include "frame/global/skillevent.h"
#include "gdi.h"

using namespace ns_frame;

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
    gdi::TabSelectType arg;
    arg.emplace_back();
    arg[0]["ID"] = std::to_string(ID);
    gdi::Interface::tabSelect(gdi::Tab::skillevent, arg);
    skillevent.tab = std::move(arg[0]);
    // 初始化数据. std::stoi() 用于确定字段存在的情况. 若该字段可能为空, 必须使用 atoi().
    skillevent.type = ref::mapSkilleventEventtype.find(skillevent.tab["EventType"]) != ref::mapSkilleventEventtype.end() ? ref::mapSkilleventEventtype.at(skillevent.tab["EventType"]) : ref::enumSkilleventEventtype::COUNT;
    skillevent.Odds = std::stoi(skillevent.tab["Odds"]);
    skillevent.SkillID = std::stoi(skillevent.tab["SkillID"]);
    skillevent.SkillLevel = std::stoi(skillevent.tab["SkillLevel"]);
    skillevent.SkillCaster = ref::mapSkilleventCastertarget.find(skillevent.tab["SkillCaster"]) != ref::mapSkilleventCastertarget.end() ? ref::mapSkilleventCastertarget.at(skillevent.tab["SkillCaster"]) : ref::enumSkilleventCastertarget::COUNT;
    skillevent.SkillTarget = ref::mapSkilleventCastertarget.find(skillevent.tab["SkillTarget"]) != ref::mapSkilleventCastertarget.end() ? ref::mapSkilleventCastertarget.at(skillevent.tab["SkillTarget"]) : ref::enumSkilleventCastertarget::COUNT;
    skillevent.EventMask1 = std::stoi(skillevent.tab["EventMask1"]);
    skillevent.EventMask2 = std::stoi(skillevent.tab["EventMask2"]);
    skillevent.EventSkillID = std::stoi(skillevent.tab["EventSkillID"]);
    // 将 SkillEvent 存入缓存
    data[ID] = std::move(skillevent);
}