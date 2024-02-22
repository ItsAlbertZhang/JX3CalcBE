#include "modules/web.h"
#include "modules/task.h"
#include "utils/config.h"
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

    CROW_ROUTE(app, "/config")
        .methods("POST"_method)([](const crow::request &req) {
            static bool isConfigAvailable = true;
            if (isConfigAvailable) {
                isConfigAvailable = false;
                ns_utils::config::initDataFromString(req.body);
                return crow::response{200, "application/json", R"({"status":0,"content":"OK"})"};
            }
            return crow::response{200, "application/json", R"({"status":-1,"content":"config is unavailable now"})"};
        });

    CROW_ROUTE(app, "/version")
        .methods("GET"_method)([]() {
            static const std::string version = "24022101";
            return crow::response{200, "text/plain", version};
        });

    CROW_ROUTE(app, "/create")
        .methods("POST"_method)([](const crow::request &req) {
            auto res = task::create(req.body);
            return crow::response{200, "application/json", res.format()};
        });

    CROW_ROUTE(app, "/query/<string>/dps")
        .methods("GET"_method)([](std::string id) {
            if (!task::server::taskMap.contains(id))
                return crow::response{200, "application/json", R"({"status":-1,"content":"error task id"})"};
            return crow::response{200, "application/json", task::server::taskMap.at(id)->queryDPS()};
        });

    CROW_ROUTE(app, "/query/<string>/damage-list")
        .methods("GET"_method)([](std::string id) {
            if (!task::server::taskMap.contains(id))
                return crow::response{200, "application/json", R"({"status":-1,"content":"error task id"})"};
            return crow::response{200, "application/json", task::server::taskMap.at(id)->queryDamageList()};
        });

    CROW_ROUTE(app, "/query/<string>/damage-analysis")
        .methods("GET"_method)([](std::string id) {
            if (!task::server::taskMap.contains(id))
                return crow::response{200, "application/json", R"({"status":-1,"content":"error task id"})"};
            return crow::response{200, "application/json", task::server::taskMap.at(id)->queryDamageAnalysis()};
        });

    app.bindaddr("0.0.0.0").port(12897).multithreaded().run();
}
