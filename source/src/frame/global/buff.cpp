#include "frame/global/buff.h"
#include "gdi.h"

using namespace ns_frame;

const Buff &BuffManager::get(int buffID, int buffLevel) {
    // 若 Buff ID 和 Level 任一不存在, 则添加
    if (data.find(std::make_tuple(buffID, buffLevel)) == data.end()) {
        add(buffID, buffLevel);
    }
    return data[std::make_tuple(buffID, buffLevel)];
}

void BuffManager::add(int buffID, int buffLevel) {
    std::lock_guard<std::mutex> lock(mutex); // 加锁
    // 可能有多个线程同时进入了 add 函数. 因此, 需要在加锁后再次判断 Buff 是否存在.
    if (data.find(std::make_tuple(buffID, buffLevel)) != data.end()) {
        // 若 Buff ID 存在, 且 Buff 等级存在, 则直接返回
        return; // 返回时, 会自动调用 lock 的析构函数, 从而释放锁
    }

    Buff buff;
    // 获取 tab
    gdi::TabSelectType arg;
    arg.emplace_back();
    arg[0]["ID"] = std::to_string(buffID);
    arg[0]["Level"] = std::to_string(buffLevel);
    gdi::Interface::tabSelect(gdi::Tab::buff, arg);
    buff.tab = std::move(arg[0]);
    // 初始化 buff
    buff.ID = buffID;
    buff.Level = buffLevel;
    buff.IsStackable = buff.tab["IsStackable"] == "1";
    buff.MaxStackNum = std::stoi(buff.tab["MaxStackNum"]);
    buff.Count = std::stoi(buff.tab["Count"]);
    buff.Interval = std::stoi(buff.tab["Interval"]);
    buff.Hide = buff.tab["Hide"] == "1";
    buff.CanCancel = buff.tab["CanCancel"] == "1";
    buff.MinInterval = std::stoi(buff.tab["MinInterval"]);
    buff.MaxInterval = std::stoi(buff.tab["MaxInterval"]);
    // 初始化 buff Attrib
    static const std::string attribName[] = {"Begin", "Active", "EndTime"};
    static int attribCount[3]; // 3 = sizeof(attribName)
    static bool isFieldCountInit = false;
    if (!isFieldCountInit) {
        // 初始化 attribCount. 当前版本下, 三个字段的数量分别为 15, 2, 2.
        for (int i = 0; i < 3; i++) { // 3 = sizeof(attribName)
            int cnt = 1;
            while (buff.tab.find(attribName[i] + "Attrib" + std::to_string(cnt)) != buff.tab.end()) { // BeginAttrib1, BeginAttrib2, ...
                cnt++;
            }
            attribCount[i] = cnt - 1;
        }
        isFieldCountInit = true;
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < attribCount[i]; j++) {
            std::string name = buff.tab[attribName[i] + "Attrib" + std::to_string(j + 1)];        // BeginAttrib1, BeginAttrib2, ...
            std::string valueA = buff.tab[attribName[i] + "Value" + std::to_string(j + 1) + "A"]; // BeginValue1A, BeginValue2A, ...
            std::string valueB = buff.tab[attribName[i] + "Value" + std::to_string(j + 1) + "B"]; // BeginValue1B, BeginValue2B, ...
            // valueB 的数字形态. 之所以不用 std::stoi, 是因为 tab 表中存在有 valueB 为字符串的情况 (与此同时 valueA 为空, 怕是策划填错了位置)
            int valueBInt = atoi(valueB.c_str());
            TabEnum::BuffAttrib type = getAttribType(name); // BeginAttrib1 的枚举类型
            std::vector<Buff::Attrib> *attrib;
            switch (i) {
            case 0:
                attrib = &buff.BeginAttrib;
                break;
            case 1:
                attrib = &buff.ActiveAttrib;
                break;
            case 2:
                attrib = &buff.EndTimeAttrib;
                break;
            }
            if (TabAdditional::buffAttribIsValue[static_cast<int>(type)]) {
                attrib->emplace_back(type, std::stoi(valueA), valueBInt);
            } else {
                attrib->emplace_back(type, valueA, valueBInt);
            }
        }
    }

    // 将 Buff 存入缓存
    data[std::make_tuple(buffID, buffLevel)] = std::move(buff);
}

void BuffManager::initAttribMap() {
    // 这一函数只会被调用一次, 且调用堆栈为 add -> getAttribType -> initAttribMap, 因此其调用时天然处于加锁状态.
    for (int i = 0; i < static_cast<int>(TabEnum::BuffAttrib::COUNT); i++) {
        attribMap[TabString::buffAttrib[i]] = static_cast<TabEnum::BuffAttrib>(i);
    }
    isAttribMapInit = true;
}

TabEnum::BuffAttrib BuffManager::getAttribType(const std::string &attribName) {
    // 这一函数只会被调用一次, 且调用堆栈为 add -> getAttribType, 因此其调用时天然处于加锁状态.
    if (!isAttribMapInit) {
        initAttribMap();
    }
    if (attribMap.find(attribName) == attribMap.end()) {
        return TabEnum::BuffAttrib::COUNT;
    }
    return attribMap[attribName];
}