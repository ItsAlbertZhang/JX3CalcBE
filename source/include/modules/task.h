#pragma once

#include "frame/character/derived/player.h"
#include "frame/character/effect.h"
#include "frame/character/property/attribute.h"
#include "frame/common/damage.h"
#include "modules/pool.h"
#include <format>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#pragma warning(push, 0)      // MSVC
#pragma clang diagnostic push // Clang
#pragma clang diagnostic ignored "-Weverything"

#ifdef _WIN32
#include <sdkddkver.h>
#endif
#include <asio/io_context.hpp>
#include <crow.h>

#pragma clang diagnostic pop // Clang
#pragma warning(pop)         // MSVC

namespace jx3calc {
namespace modules {

namespace task {

class Task {
public:
    Task(const std::string &id)
        : id(id){};
    const std::string id;

    class Data {
    public:
        ~Data();
        concrete::Player playerType;

        int delayNetwork;
        int delayKeyboard;
        int fightTime;
        int fightCount;

        frame::ChAttr                               attrBackup;
        std::vector<std::shared_ptr<frame::Effect>> effects;

        // 战斗类型. -1 代表自定义战斗(不使用内置), 非负数代表使用内置, 且数值等于内置枚举索引, 其中 0 为默认内置循环.
        // 具体数值代表的状态由各心法负责具体实现.
        int fightType = 0;

        frame::Player::typeSkillMap    skills;
        frame::Player::typeTalentArray talents;

        std::optional<std::string> fight;

    } data;

    std::atomic<bool>             stop{false};
    std::vector<std::future<int>> futures;
    std::mutex                    mutex; // 用于保护 results 和 details, 被 io 线程和 web 线程同时访问
    std::vector<int>              results;
    std::vector<frame::ChDamage>  details;
    int                           cntCompleted = 0; // 已完成的任务数, 同时也是当前任务的 idx
    int                           speedCurr    = 0; // 当前速度

    std::string queryDPS();
    std::string queryDamageList();
    std::string queryDamageAnalysis();
    std::string queryDamageChart();
};

enum class enumAttributeType {
    data,
    jx3box,
};
inline const std::unordered_map<std::string, enumAttributeType> refAttributeType{
    {"从数据导入", enumAttributeType::data},
    {"从JX3BOX导入", enumAttributeType::jx3box},
};

enum class enumCustom {
    // none,
    lua,
    jx3,
};
inline std::unordered_map<std::string, enumCustom> refCustom{
    // {"使用内置循环", enumCustom::none},
    {"使用lua编程语言", enumCustom::lua},
    {"使用游戏内宏", enumCustom::jx3},
};

class Response {
    inline static const char *status[]{
        "",
        "config.json not available.",
        "json parse error.",
        "Error in base: missing field or invalid option: ",
        "Error in base: invalid input value.",
        "Error in attribute: ",
        "Error in effect: ",
        "Error in fight: ",
        "Error in talents: ",
        "Error in skills: ",
    };

public:
    int         idx = 1;
    std::string message;

    void next() {
        assert(idx > 0);
        idx++;
        if (idx == sizeof(status) / sizeof(status[0])) [[unlikely]]
            idx = 0;
    }
    std::string format() {
        return std::format("{{\"status\":{},\"data\":\"{}{}\"}}", idx, status[idx], message);
    }
};

class Server {
public:
    Server();  // 构造函数会非阻塞异步启动子线程任务模块.
    ~Server(); // 析构函数会停止子线程任务模块并同步等待其退出.
    auto validate(const std::string &jsonstr) -> Response;
    auto create(const std::string &jsonstr) -> Response;
    void pause(std::string id);
    void stop(std::string id);

    std::unordered_map<std::string, std::unique_ptr<Task>> taskMap;
    Pool                                                   pool;

private:
    std::thread      ioThread;
    asio::io_context ioContext;
};

} // namespace task

} // namespace modules
} // namespace jx3calc
