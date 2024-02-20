#include "modules/web.h"
#include "modules/task.h"
#include "utils/config.h"
#include "utils/json_schema.h"
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
    CROW_ROUTE(app, "/")
        .methods("GET"_method)([]() {
            // crow::mustache::set_base((ns_utils::config::pExeDir / "templates").string());
            // crow::mustache::context    x;
            // crow::mustache::template_t page = crow::mustache::load("index.html");
            // crow::response             res{page.render(x)};
            crow::response res{200, R"(<h1>Hello, world!</h1>)"};
            res.add_header("Content-Type", "text/html; charset=utf-8");
            return res;
        });

    CROW_ROUTE(app, "/setting")
        .methods("POST"_method)([](const crow::request &req) {
            UNREFERENCED_PARAMETER(req);
            return crow::response{400, "text/plain", "setting is unavailable now."};
        });

    CROW_ROUTE(app, "/task")
        .methods("GET"_method)([]() {
            return crow::response{200, "application/json", ns_utils::config::taskdata::genSchema()};
        });

    CROW_ROUTE(app, "/task/validate")
        .methods("POST"_method)([](const crow::request &req) {
            if (req.body.empty())
                return crow::response{400};
            else {
                using json = nlohmann::json;
                json j;
                try {
                    j = json::parse(req.body);
                } catch (...) {
                    return crow::response{400, "json parse error."};
                }
                if (ns_utils::json_schema::validate(json::parse(ns_utils::config::taskdata::genSchema()), j))
                    return crow::response{200, "OK"};
                else
                    return crow::response{400, "json invalid."};
            }
        });

    CROW_ROUTE(app, "/task")
        .methods("POST"_method)([](const crow::request &req) {
            auto id = task::create(req.body);
            return crow::response{200, id.format()};
        });

    CROW_WEBSOCKET_ROUTE(app, "/task/ws")
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
