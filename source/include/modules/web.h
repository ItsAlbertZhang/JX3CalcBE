#ifndef PROGRAM_THREAD_WEB_H_
#define PROGRAM_THREAD_WEB_H_

#pragma warning(push, 0)
#ifdef _WIN32
#include <sdkddkver.h>
#endif
#include <crow.h>
#include <crow/middlewares/cors.h>
#include <crow/websocket.h>
#pragma warning(pop)

#include "modules/pool.h"
#include "modules/task.h"
#include <asio.hpp>
#include <string>
#include <thread>
#include <unordered_map>

namespace ns_program {

class Web {
public:
    Web(); // 构造 WebHandler 的同时, 会启动一个线程
    virtual ~Web();
    Web(const Web &)            = delete;
    Web &operator=(const Web &) = delete;
    Web(Web &&)                 = delete;
    Web &operator=(Web &&)      = delete;

    void stop();

private:
    std::thread webThread;
    void        webEntry();
    std::thread ioThread;
    Pool        pool;

    crow::App<crow::CORSHandler> app;

    std::unordered_map<std::string, Task>                     tasks;
    std::unordered_map<crow::websocket::connection *, Task *> wsmap;

    asio::io_context  io;
    std::atomic<bool> iostop{false};

    std::string urlTask(const std::string &jsonstr);
    void        urlTaskWs_onMessage(crow::websocket::connection &conn, const std::string &data, bool is_binary);
    void        urlTaskWs_onClose(crow::websocket::connection &conn);
    std::string genID(int length = 6);
};

} // namespace ns_program

#endif // PROGRAM_THREAD_WEB_H_
