#include "modules/web.h"
#include "modules/task.h"
#include "utils/config.h"
#include <asio.hpp>
#include <crow/mustache.h>
#include <string>
#include <thread>

#define UNREFERENCED_PARAMETER(P) (P)

using namespace ns_modules;

void web::run() {
    task::server::asyncrun();
    threadApp = std::thread(&web::threadAppEntry);
    managerEntry(); // 阻塞
    task::server::stop();
    app.stop();
    threadApp.join();
}

void web::managerEntry() {
    CROW_ROUTE(manager, "/config")
        .methods("POST"_method)([](const crow::request &req) {
            ns_utils::config::initDataFromString(req.body);
            return crow::response{200};
        });

    CROW_ROUTE(manager, "/stop")
        .methods("GET"_method)([]() {
            manager.stop(); // 退出阻塞
            return crow::response{200};
        });
    manager.bindaddr("0.0.0.0").port(12898).run();
}

void web::threadAppEntry() {
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
