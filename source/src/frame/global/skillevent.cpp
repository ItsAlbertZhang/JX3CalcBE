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
    gdi::select_t arg;
    arg.emplace_back();
    arg[0]["ID"] = std::to_string(ID);
    gdi::tabSelect(gdi::Tab::skillevent, arg);
    skillevent.tab = std::move(arg[0]);

    using eType = ref::enumSkilleventEventtype;
    using eCt   = ref::enumSkilleventCastertarget;

    const std::unordered_map<std::string, eType> &mType = ref::mapSkilleventEventtype;
    const std::unordered_map<std::string, eCt>   &mCt   = ref::mapSkilleventCastertarget;

    // 初始化数据. std::stoi() 用于确定字段存在的情况. 若该字段可能为空, 必须使用 atoi().
    skillevent.type         = mType.find(skillevent.tab["EventType"]) != mType.end() ? mType.at(skillevent.tab["EventType"]) : eType::COUNT;
    skillevent.Odds         = std::stoi(skillevent.tab["Odds"]);
    skillevent.SkillID      = std::stoi(skillevent.tab["SkillID"]);
    skillevent.SkillLevel   = std::stoi(skillevent.tab["SkillLevel"]);
    skillevent.SkillCaster  = mCt.find(skillevent.tab["SkillCaster"]) != mCt.end() ? mCt.at(skillevent.tab["SkillCaster"]) : eCt::COUNT;
    skillevent.SkillTarget  = mCt.find(skillevent.tab["SkillTarget"]) != mCt.end() ? mCt.at(skillevent.tab["SkillTarget"]) : eCt::COUNT;
    skillevent.EventMask1   = std::stoi(skillevent.tab["EventMask1"]);
    skillevent.EventMask2   = std::stoi(skillevent.tab["EventMask2"]);
    skillevent.EventSkillID = std::stoi(skillevent.tab["EventSkillID"]);
    // 将 SkillEvent 存入缓存
    data[ID] = std::move(skillevent);
}
