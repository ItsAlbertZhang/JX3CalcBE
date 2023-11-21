#ifndef FRAME_GLOBAL_BUFF_H_
#define FRAME_GLOBAL_BUFF_H_

#include "frame/tab_static.h"
#include <mutex>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace ns_frame {

/**
 * @brief Buff 类
 * @warning Buff 是公共资源, 不属于某个角色. 应当使用 BuffManager 类对其统一进行存取.
 */
class Buff {
public:
    // ---------- 数据存放区 ----------
    std::unordered_map<std::string, std::string> tab; // buffs.tab 中的数据

    int ID;
    int nLevel;

    class Attrib {
    public:
        Attrib(TabEnum::BuffAttrib type, int value) : type(type), valueInt(value) {}
        Attrib(TabEnum::BuffAttrib type, std::string value) : type(type), valueStr(value) {}
        TabEnum::BuffAttrib type = TabEnum::BuffAttrib::COUNT;
        int valueInt = 0;
        std::string valueStr = "";
    };
    std::vector<Attrib> BeginAttrib;
    std::vector<Attrib> ActiveAttrib;
    std::vector<Attrib> EndTimeAttrib;
};

/**
 * @brief Buff 管理类
 * @warning 这是一个静态类, 用于管理 Buff 数据, 不应当被创建实例.
 */
class BuffManager {
public:
    BuffManager() = delete;

    /**
     * @brief 获取 Buff. 若 Buff 存在, 则命中缓存并返回 Buff 数据; 若 Buff 不存在, 则对其进行初始化并返回 Buff 数据.
     * @param buffID
     * @param buffLevel
     * @return Buff&
     */
    static Buff &get(int buffID, int buffLevel);

private:
    static inline std::mutex mutex; // 互斥锁. 用于保护 add 操作.

    struct tuple_hash {
        template <class T1, class T2>
        std::size_t operator()(const std::tuple<T1, T2> &t) const {
            auto h1 = std::hash<T1>{}(std::get<0>(t));
            auto h2 = std::hash<T2>{}(std::get<1>(t));
            return h1 ^ h2;
        }
    };

    /**
     * @brief Buff 缓存数据
     * @note 同一 ID, 不同 Level 的 Buff 拥有不同的 Buff 实例.
     */
    static inline std::unordered_map<std::tuple<int, int>, Buff, tuple_hash> data;

    /**
     * @brief 初始化技能. 将指定 ID 与 Level 的 Buff 数据存至缓存.
     * @param buffID
     * @param buffLevel
     */
    static void add(int buffID, int buffLevel);
};

} // namespace ns_frame

#endif // FRAME_GLOBAL_BUFF_H_