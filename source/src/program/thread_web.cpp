#include "program/thread_web.h"
#include "program/task.h"
#include <asio.hpp>
#include <chrono>
#include <crow/http_response.h>
#include <crow/logging.h>
#include <crow/mustache.h>
#include <string>
#include <thread>

#define UNREFERENCED_PARAMETER(P) (P)

using namespace ns_program;

Web::Web() {
    asio::co_spawn(
        io,
        [&]() -> asio::awaitable<void> {
            asio::steady_timer timer(io);
            while (!iostop.load()) {
                timer.expires_after(std::chrono::seconds(1));
                co_await timer.async_wait(asio::use_awaitable);
            }
        },
        asio::detached
    ); // 用于保持 io.run() 的运行
    webThread = std::thread(&Web::webEntry, this);
    ioThread  = std::thread([&]() {
        io.run();
    });
}

Web::~Web() {
    for (auto &task : tasks)
        task.second.stop.store(true);
    iostop.store(true);
    webThread.join();
    ioThread.join();
}

void Web::webEntry() {
    CROW_ROUTE(app, "/")
        .methods("GET"_method)([]() {
            crow::mustache::context    x;
            crow::mustache::template_t page = crow::mustache::load("index.html");
            crow::response             res{page.render(x)};
            res.add_header("Content-Type", "text/html; charset=utf-8");
            return res;
        });

    CROW_ROUTE(app, "/setting")
        .methods("POST"_method)([this](const crow::request &req) {
            UNREFERENCED_PARAMETER(req);
            return crow::response{400, "text/plain", "setting is unavailable now."};
        });

    CROW_ROUTE(app, "/task")
        .methods("GET"_method)([this]() {
            return crow::response{200, "application/json", DMTask::format()};
        });

    CROW_ROUTE(app, "/task")
        .methods("POST"_method)([this](const crow::request &req) {
            std::string id = urlTask(req.body);
            if (id.empty())
                return crow::response{400};
            else
                return crow::response{200, id};
        });

#ifdef _WIN32
    CROW_ROUTE(app, "/ws")
        .websocket()
#else
    CROW_WEBSOCKET_ROUTE(app, "/task/ws")
#endif
        .onopen([&](crow::websocket::connection &conn) {
            CROW_LOG_INFO << "a websocket opened.";
        })
        .onclose([&](crow::websocket::connection &conn, const std::string &reason) {
            CROW_LOG_INFO << "a websocket closed.";
            urlTaskWs_onClose(conn);
        })
        .onmessage([&](crow::websocket::connection &conn, const std::string &data, bool is_binary) {
            urlTaskWs_onMessage(conn, data, is_binary);
        });

    app.bindaddr("127.0.0.1").port(12897).multithreaded().run();
}

void Web::stop() {
    app.stop();
}

std::string Web::genID(int length) {
    const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device              random_device;
    std::mt19937                    generator(random_device());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

    std::string random_string;
    while (random_string.empty() || tasks.contains(random_string)) {
        for (size_t i = 0; i < length; ++i) {
            random_string += CHARACTERS[distribution(generator)];
        }
    }
    return random_string;
}
