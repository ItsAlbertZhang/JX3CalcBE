#ifndef MODULES_TASK_H_
#define MODULES_TASK_H_

#include "concrete/effect/base.h"
#include "frame/character/property/attribute.h"
#include "frame/character/property/damage.h"
#include "frame/custom/base.h"
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

namespace ns_modules {

namespace task {

class Data {
public:
    // json 数据区
    int delayNetwork;
    int delayKeyboard;
    int fightTime;
    int fightCount;

    ns_frame::ChAttr                                      attrBackup;
    std::vector<std::shared_ptr<ns_concrete::EffectBase>> effects;

    ns_frame::enumCustom customType;
    std::string          customString;
};

class Task {
public:
    Task(const std::string &id, const Data &data)
        : id(id), data(data){};
    const std::string               id;
    const Data                      data;
    std::atomic<bool>               stop{false};
    std::vector<std::future<int>>   futures;
    std::mutex                      mutex; // 用于保护 results 和 details, 被 io 线程和 web 线程同时访问
    std::vector<int>                results;
    std::vector<ns_frame::ChDamage> details;
    int                             cntCompleted = 0; // 已完成的任务数, 同时也是当前任务的 idx
    int                             speedCurr    = 0; // 当前速度

    std::string queryDPS();
    std::string queryDamageList();
    std::string queryDamageAnalysis();
    std::string queryDamageChart();
};

enum class AttributeType {
    data,
    jx3box,
    COUNT,
};
inline const std::unordered_map<std::string, AttributeType> refAttributeType{
    {"从数据导入", AttributeType::data  },
    {"从JX3BOX导入", AttributeType::jx3box},
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

} // namespace ns_modules

#endif // MODULES_TASK_H_
