#include "modules/web.h"
#include "modules/config.h"
#include "modules/task.h"
#include <future>
#include <string>

#pragma warning(push, 0)      // MSVC
#pragma clang diagnostic push // Clang
#pragma clang diagnostic ignored "-Weverything"

#ifdef _WIN32
#include <sdkddkver.h>
#endif
#include <crow.h>
#include <crow/middlewares/cors.h>

#pragma clang diagnostic pop // Clang
#pragma warning(pop)         // MSVC

void jx3calc::modules::web::run() {
    std::atomic<bool>            stop {false};
    crow::App<crow::CORSHandler> app;
    crow::App<crow::CORSHandler> manager;
    task::Server                 server;

    CROW_ROUTE(app, "/status")
        .methods("GET"_method)([]() {
            return crow::response {200, "application/json", config::status()};
        });

    CROW_ROUTE(app, "/create")
        .methods("POST"_method)([&server](const crow::request &req) {
            return crow::response {200, "application/json", server.create(req.body)};
        });

    CROW_ROUTE(app, "/query/<string>/dps")
        .methods("GET"_method)([&server](std::string id) {
            if (!server.taskMap.contains(id))
                return crow::response {200, "application/json", R"({"status":-1,"data":"error task id"})"};
            return crow::response {200, "application/json", server.taskMap.at(id)->queryDPS()};
        });

    CROW_ROUTE(app, "/query/<string>/damage-list")
        .methods("GET"_method)([&server](std::string id) {
            if (!server.taskMap.contains(id))
                return crow::response {200, "application/json", R"({"status":-1,"data":"error task id"})"};
            return crow::response {200, "application/json", server.taskMap.at(id)->queryDamageList()};
        });

    CROW_ROUTE(app, "/query/<string>/damage-analysis")
        .methods("GET"_method)([&server](std::string id) {
            if (!server.taskMap.contains(id))
                return crow::response {200, "application/json", R"({"status":-1,"data":"error task id"})"};
            return crow::response {200, "application/json", server.taskMap.at(id)->queryDamageAnalysis()};
        });

#ifdef CROW_ENABLE_SSL
    app.ssl_file("fullchain.pem", "privkey.pem");
#endif
    auto futureApp = app.bindaddr("0.0.0.0").port(12897).multithreaded().run_async();

    CROW_ROUTE(manager, "/config")
        .methods("POST"_method)([&stop](const crow::request &req) {
            bool ret = modules::config::configure(req.body);
            if (ret) {
                stop.store(true);
                return crow::response {200};
            } else {
                return crow::response {400};
            }
        });

    CROW_ROUTE(manager, "/stop")
        .methods("GET"_method)([&stop]() {
            stop.store(true);
            return crow::response {200};
        });
    auto futureManager = manager.bindaddr("127.0.0.1").port(12898).run_async();

    while (!stop.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    app.stop();
    futureApp.get();
    manager.stop();
    futureManager.get();
}
