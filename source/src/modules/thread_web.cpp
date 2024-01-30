#include "modules/task.h"
#include "modules/web.h"
#include "utils/config.h"
#include "utils/json_schema.h"
#include <asio.hpp>
#include <chrono>
#include <crow/mustache.h>
#include <iostream>
#include <string>
#include <thread>

#define UNREFERENCED_PARAMETER(P) (P)

using namespace ns_modules;

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
    std::cout << "成功于 http://127.0.0.1:12897 上开启服务器, 按住 Ctrl 点击链接即可在浏览器中打开使用." << std::endl;
    std::cout << "Server started at http://127.0.0.1:12897 ." << std::endl;
    CROW_ROUTE(app, "/")
        .methods("GET"_method)([]() {
            crow::mustache::set_base((ns_utils::config::pExeDir / "templates").string());
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
            return crow::response{200, "application/json", task::schema()};
        });

    CROW_ROUTE(app, "/validate")
        .methods("POST"_method)([this](const crow::request &req) {
            if (req.body.empty())
                return crow::response{400};
            else {
                nlohmann::json j;
                try {
                    j = nlohmann::json::parse(req.body);
                } catch (...) {
                    return crow::response{400, "json parse error."};
                }
                if (ns_utils::json_schema::validate(ns_utils::config::schemaTaskData, j))
                    return crow::response{200, "OK"};
                else
                    return crow::response{400, "json invalid."};
            }
        });

    CROW_ROUTE(app, "/task")
        .methods("POST"_method)([this](const crow::request &req) {
            std::string id = urlTask(req.body);
            if (id.empty())
                return crow::response{400};
            else
                return crow::response{200, id};
        });

    CROW_WEBSOCKET_ROUTE(app, "/task/ws")
        .onopen([&](crow::websocket::connection &conn) {
            UNREFERENCED_PARAMETER(conn);
            CROW_LOG_INFO << "a websocket opened.";
        })
        .onclose([&](crow::websocket::connection &conn, const std::string &reason) {
            UNREFERENCED_PARAMETER(reason);
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
    std::uniform_int_distribution<> distribution(0, static_cast<int>(CHARACTERS.size() - 1));

    std::string random_string;
    while (random_string.empty() || tasks.contains(random_string)) {
        for (size_t i = 0; i < length; ++i) {
            random_string += CHARACTERS[distribution(generator)];
        }
    }
    return random_string;
}
