#include "program/thread_web.h"
#include "program/task.h"
#include <string>
#include <thread>

#define UNREFERENCED_PARAMETER(P) (P)

using namespace ns_program;

Web::Web() {
    webThread = std::thread(&Web::webEntry, this);
}

Web::~Web() {
    webThread.join();
}

void Web::webEntry() {
    CROW_ROUTE(app, "/")
        .methods("GET"_method)([]() {
            return "Hello, world!";
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
        .onclose([&](crow::websocket::connection &conn, const std::string &reason) {
            CROW_LOG_INFO << "websocket connection closed: " << reason;
        })
        .onmessage([&](crow::websocket::connection &conn, const std::string &data, bool is_binary) {
            if (!is_binary)
                std::cout << "message from client: " << data << std::endl;
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
