#ifndef MODULES_WEB_H_
#define MODULES_WEB_H_

#pragma warning(push, 0)
#ifdef _WIN32
#include <sdkddkver.h>
#endif
#include <crow.h>
#include <crow/middlewares/cors.h>
#include <crow/websocket.h>
#pragma warning(pop)

#include <asio.hpp>
#include <thread>

namespace ns_modules {

namespace web {

inline crow::App<crow::CORSHandler> app;
inline std::thread                  threadApp;
void                                threadAppEntry(); // 在另一个线程中进入, 因此是非阻塞的.

inline crow::App<crow::CORSHandler> manager;
void                                managerEntry(); // 在当前线程中进入, 因此是阻塞的.

void run(); // 阻塞式启动 web 服务.

} // namespace web

} // namespace ns_modules

#endif // MODULES_WEB_H_
