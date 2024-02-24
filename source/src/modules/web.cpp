#include "modules/web.h"
#include "modules/task.h"
#include "utils/config.h"
#include <asio.hpp>
#include <crow/mustache.h>
#include <string>

#define UNREFERENCED_PARAMETER(P) (P)

using namespace ns_modules;

web::Web::~Web() {
    app.stop();
    future.get();
}

web::WebApp::WebApp() {
    task::server::asyncrun();
    CROW_ROUTE(app, "/version")
        .methods("GET"_method)([]() {
            static const std::string version = "24022501";
            return crow::response{200, "text/plain", version};
        });

    CROW_ROUTE(app, "/available")
        .methods("GET"_method)([]() {
            const std::string t = R"({"status":0})";
            const std::string f = R"({"status":-1})";
            return crow::response{200, "application/json", ns_utils::config::dataAvailable ? t : f};
        });

    CROW_ROUTE(app, "/create")
        .methods("POST"_method)([](const crow::request &req) {
            auto res = task::create(req.body);
            return crow::response{200, "application/json", res.format()};
        });

    CROW_ROUTE(app, "/query/<string>/dps")
        .methods("GET"_method)([](std::string id) {
            if (!task::server::taskMap.contains(id))
                return crow::response{200, "application/json", R"({"status":-1,"data":"error task id"})"};
            return crow::response{200, "application/json", task::server::taskMap.at(id)->queryDPS()};
        });

    CROW_ROUTE(app, "/query/<string>/damage-list")
        .methods("GET"_method)([](std::string id) {
            if (!task::server::taskMap.contains(id))
                return crow::response{200, "application/json", R"({"status":-1,"data":"error task id"})"};
            return crow::response{200, "application/json", task::server::taskMap.at(id)->queryDamageList()};
        });

    CROW_ROUTE(app, "/query/<string>/damage-analysis")
        .methods("GET"_method)([](std::string id) {
            if (!task::server::taskMap.contains(id))
                return crow::response{200, "application/json", R"({"status":-1,"data":"error task id"})"};
            return crow::response{200, "application/json", task::server::taskMap.at(id)->queryDamageAnalysis()};
        });

    future = app.bindaddr("0.0.0.0").port(12897).multithreaded().run_async();
}

web::WebApp::~WebApp() {
    task::server::stop();
}

web::WebManager::WebManager() {
    CROW_ROUTE(app, "/config")
        .methods("POST"_method)([](const crow::request &req) {
            ns_utils::config::initDataFromString(req.body);
            return crow::response{200};
        });

    CROW_ROUTE(app, "/stop")
        .methods("GET"_method)([]() {
            stop.store(true);
            return crow::response{200};
        });
    future = app.bindaddr("0.0.0.0").port(12898).run_async();
}

void web::run() {
    WebApp     app;
    WebManager manager;
    while (!stop.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    // 离开作用域, app 和 manager 自动析构.
}
