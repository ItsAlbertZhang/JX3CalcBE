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
    task::server::run();
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

    CROW_ROUTE(app, "/query/<string>")
        .methods("GET"_method)([](std::string id) {
            return crow::response{200, "text/plain", task::server::taskMap.at(id)->format()};
        });

    app.bindaddr("0.0.0.0").port(12897).multithreaded().run();
}
