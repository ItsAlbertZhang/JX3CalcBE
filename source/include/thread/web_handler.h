#ifndef THREAD_MAIN_H_
#define THREAD_MAIN_H_

#include "thread/pool.h"
#include "thread/web_helper.h"
#pragma warning(push, 0)
#include <crow.h>
#pragma warning(pop)
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

namespace ns_thread {

class WebHandler {
public:
    WebHandler();
    virtual ~WebHandler();
    WebHandler(const WebHandler &)            = delete;
    WebHandler &operator=(const WebHandler &) = delete;
    WebHandler(WebHandler &&)                 = delete;
    WebHandler &operator=(WebHandler &&)      = delete;

    void stop();

private:
    std::unique_ptr<std::thread> thread;

    WebHelper helper;
    Pool      pool;

    friend class WebHelper;

    // web server
    crow::SimpleApp app;
    std::mutex      mtx;
    std::unordered_map<crow::websocket::connection *, std::string>
        wsmap;

    void run();
    bool task(const std::string &jsonstr);
};

} // namespace ns_thread

#endif // THREAD_MAIN_H_
