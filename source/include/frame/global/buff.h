#ifndef FRAME_GLOBAL_BUFF_H_
#define FRAME_GLOBAL_BUFF_H_

#include "frame/ref/tab_attribute.h" // ref::enumTabAttribute
#include <mutex>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace ns_frame {

/**
 * @brief Buff 类
 * @warning Buff 是公共资源, 不属于某个角色. 应当使用 BuffManager 类对其统一进行存取.
 * @warning #### 关于线程安全以及实现方式:
 * @warning - 这个类的实例会在多线程之间共享.
 * @warning - 在构造以外的情况下, 这个类的实例 **应当是** 只读的.
 * @warning - 这个类实例的构造由 `Manager::add()` 完成, 并借助锁确保线程安全.
 */
class Buff {
public:
    // ---------- 数据存放区 ----------
    std::unordered_map<std::string, std::string> tab; // buffs.tab 中的数据

    // 初始化时拿出一些数据, 降低使用开销
    int ID;
    int Level;
    bool IsStackable;
    int MaxStackNum;
    int Count;
    int Interval;
    bool Hide;
    bool Exclude;
    std::string ScriptFile;
    bool CanCancel;
    int MinInterval;
    int MaxInterval;

    class Attrib {
    public:
        Attrib(ref::enumTabAttribute type, const std::string &valueA, const std::string &valueB) : type(type), valueAStr(valueA), valueBStr(valueB) {}
        ref::enumTabAttribute type = ref::enumTabAttribute::COUNT;
        std::string valueAStr = "";
        std::string valueBStr = "";
        int valueAInt = 0;
        int valueBInt = 0;
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
    static const Buff &get(int buffID, int buffLevel);

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