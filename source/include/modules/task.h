#ifndef MODULES_TASK_H_
#define MODULES_TASK_H_

#include "concrete/effect.h"
#include "concrete/player.h"
#include "frame/character/property/attribute.h"
#include "frame/character/property/damage.h"
#include "modules/pool.h"
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
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

class Data {
public:
    ~Data();

    concrete::player::Type playerType;

    int delayNetwork;
    int delayKeyboard;
    int fightTime;
    int fightCount;

    frame::ChAttr                                        attrBackup;
    std::vector<std::shared_ptr<concrete::effect::Base>> effects;

    std::string customString;
};

class Task {
public:
    Task(const std::string &id, const Data &data)
        : id(id), data(data){};
    const std::string             id;
    const Data                    data;
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
    COUNT,
};
inline const std::unordered_map<std::string, enumAttributeType> refAttributeType{
    {"从数据导入", enumAttributeType::data  },
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
    {"使用游戏内宏",    enumCustom::jx3},
};

enum class ResponseStatus {
    success,
    config_error,
    parse_error,
    missing_field,
    invalid_field,
    invalid_player,
    invalid_interger,
    invalid_attribute_method,
    invalid_attribute_data,
    invalid_effects,
    invalid_custom,
    create_data_error,
};
class Response {
public:
    ResponseStatus status;
    std::string    data;
    std::string    format() {
        nlohmann::json j;
        j["status"] = static_cast<int>(status);
        j["data"]   = data;
        return j.dump();
    }
};
Response validate(const std::string &jsonstr);
Response create(const std::string &jsonstr);
void     pause(std::string id);
void     stop(std::string id);

namespace server {

inline std::unordered_map<std::string, std::unique_ptr<Task>> taskMap;

void asyncrun(); // 非阻塞异步启动子线程任务模块.
void stop();     // 停止子线程任务模块并同步等待其退出.

inline Pool pool;
/**
 *  注意, pool 的构造晚于 taskMap. 这很重要:
 *
 */

inline std::thread      ioThread;
inline asio::io_context ioContext;

} // namespace server

} // namespace task

} // namespace modules
} // namespace jx3calc

#endif // MODULES_TASK_H_
