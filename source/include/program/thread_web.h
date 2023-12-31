#ifndef PROGRAM_THREAD_WEB_H_
#define PROGRAM_THREAD_WEB_H_

#include "program/task.h"
#include "program/thread_pool.h"
#include <asio.hpp>
#include <crow/websocket.h>
#include <unordered_map>
#pragma warning(push, 0)
#include <crow.h>
#include <crow/middlewares/cors.h>
#pragma warning(pop)
#include <string>
#include <thread>

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
