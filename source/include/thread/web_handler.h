#ifndef THREAD_WEB_HANDLER_H_
#define THREAD_WEB_HANDLER_H_

#include "thread/pool.h"
#include "thread/web_datashared.h"
#include "thread/web_helper.h"
#pragma warning(push, 0)
#include <crow.h>
#pragma warning(pop)
#include <memory>
#include <string>
#include <thread>

namespace ns_thread {

class WebHandler {
public:
    WebHandler(); // 构造 WebHandler 的同时, 会启动一个线程
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

    WebDataShared shared;

    crow::SimpleApp app;

    void run();
    bool task(const std::string &jsonstr);
};

} // namespace ns_thread

#endif // THREAD_WEB_HANDLER_H_
