#include "thread/web_handler.h"
#include <string>
#include <thread>
#include <unordered_map>

using namespace ns_thread;

WebHandler::WebHandler() {
    thread = std::make_unique<std::thread>(&WebHandler::run, this);
}

WebHandler::~WebHandler() {
    thread->join();
}

void WebHandler::run() {
    CROW_ROUTE(app, "/")
        .methods("GET"_method)([]() {
            return "Hello, world!";
        });

    CROW_ROUTE(app, "/setting")
        .methods("POST"_method)([this](const crow::request &req) {
            return crow::response{400, "text/plain", "setting is unavailable now."};
        });

    CROW_ROUTE(app, "/task")
        .methods("GET"_method)([this]() {
            return crow::response{200, "text/plain", "task getting is unavailable now."};
        });

    CROW_ROUTE(app, "/task")
        .methods("POST"_method)([this](const crow::request &req) {
            this->task(req.body);
            return crow::response{200};
        });

    CROW_WEBSOCKET_ROUTE(app, "/ws")
        .onclose([&](crow::websocket::connection &conn, const std::string &reason) {
            CROW_LOG_INFO << "websocket connection closed: " << reason;
            std::lock_guard<std::mutex> _(mtx);
            wsmap.erase(&conn);
        })
        .onmessage([&](crow::websocket::connection &conn, const std::string &data, bool is_binary) {
            std::cout << "message from client: " << data << std::endl;
            std::lock_guard<std::mutex> _(mtx);
            if (!is_binary)
                wsmap.emplace(&conn, data);
        });

    app.bindaddr("127.0.0.1").port(12897).multithreaded().run();
}

void WebHandler::stop() {
    app.stop();
}
