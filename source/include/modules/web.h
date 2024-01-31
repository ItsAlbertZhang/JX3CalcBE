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

inline std::thread threadWeb;
void               entry();

inline crow::App<crow::CORSHandler> app;

void run();
void stop();

} // namespace web

} // namespace ns_modules

#endif // MODULES_WEB_H_
