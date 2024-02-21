#include "modules/web.h"
#include "modules/task.h"
#include <asio.hpp>
#include <crow/mustache.h>
#include <iostream>
#include <string>
#include <thread>

#define UNREFERENCED_PARAMETER(P) (P)

using namespace ns_modules;

void web::run() {
    threadWeb = std::thread(&web::entry);
}

void web::stop() {
    task::server::stop();
    app.stop();
    threadWeb.join();
}

void web::entry() {
    std::cout << "成功于 http://127.0.0.1:12897 上开启服务器, 按住 Ctrl 点击链接即可在浏览器中打开使用." << std::endl;
    std::cout << "Server started at http://127.0.0.1:12897 ." << std::endl;

    CROW_ROUTE(app, "/setting")
        .methods("POST"_method)([](const crow::request &req) {
            UNREFERENCED_PARAMETER(req);
            return crow::response{400, "text/plain", "setting is unavailable now."};
        });

    CROW_ROUTE(app, "/version")
        .methods("GET"_method)([]() {
            static const std::string version = "24022101";
            return crow::response{200, "text/plain", version};
        });

    CROW_ROUTE(app, "/create")
        .methods("POST"_method)([](const crow::request &req) {
            auto id = task::create(req.body);
            return crow::response{200, id.format()};
        });

    CROW_WEBSOCKET_ROUTE(app, "/ws")
        .onopen([&](crow::websocket::connection &conn) {
            CROW_LOG_INFO << "Websocket connected.";
            task::server::run(&conn);
        })
        .onclose([&](crow::websocket::connection &conn, const std::string &reason) {
            UNREFERENCED_PARAMETER(reason);
            CROW_LOG_INFO << "Websocket disconnected.";
            task::server::stop();
        });

    app.bindaddr("0.0.0.0").port(12897).multithreaded().run();
}
