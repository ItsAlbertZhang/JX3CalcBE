#ifndef MODULES_TASK_H_
#define MODULES_TASK_H_

#include "concrete/effect/base.h"
#include "frame/character/property/attribute.h"
#include "frame/character/property/damage.h"
#include "modules/pool.h"
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#pragma warning(push, 0)
#ifdef _WIN32
#include <sdkddkver.h>
#endif
#include <crow.h>
#pragma warning(pop)

namespace ns_modules {

namespace web {
namespace task {

class Data {
public:
    // json 数据区
    int delayNetwork;
    int delayKeyboard;
    int fightTime;
    int fightCount;

    bool        useCustomMacro = false;
    std::string customMacro;

    ns_frame::ChAttr                                      attrBackup;
    std::vector<std::shared_ptr<ns_concrete::EffectBase>> effects;

    // 根据 json 数据区计算得到的数据区
    size_t custom_macro_hash = std::hash<std::string>{}(customMacro);
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
    // zero,
    jx3box,
    COUNT,
};
inline const std::unordered_map<std::string, AttributeType> refAttributeType{
  // {"未启用",       AttributeType::jx3box},
    {"从JX3BOX导入", AttributeType::jx3box},
};

enum class ResponseStatus {
    success,
    parse_error,
    missing_field,
    invalid_player,
    invalid_interger,
    invalid_attribute_method,
    invalid_attribute_data,
    invalid_effects,
    create_data_error,
};
class Response {
public:
    ResponseStatus status;
    std::string    content;
    std::string    format() {
        nlohmann::json j;
        j["status"]  = static_cast<int>(status);
        j["content"] = content;
        return j.dump();
    }
};
Response validate(const std::string &jsonstr);
Response create(const std::string &jsonstr);
void     pause(std::string id);
void     stop(std::string id);

namespace server {

inline std::unordered_map<std::string, std::unique_ptr<Task>> taskMap;

void run();  // 开始运行任务模块
void stop(); // 停止任务模块

inline Pool              pool;
inline std::thread       threadIO;
inline asio::io_context  io;
inline std::atomic<bool> iostop{false};

} // namespace server

} // namespace task
} // namespace web

} // namespace ns_modules

#endif // MODULES_TASK_H_
