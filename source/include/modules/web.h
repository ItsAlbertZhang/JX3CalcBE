#ifndef MODULES_WEB_H_
#define MODULES_WEB_H_

#include <future>

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

namespace jx3calc {
namespace modules {

namespace web {

class Web {
public:
    virtual ~Web();

protected:
    crow::App<crow::CORSHandler> app;
    std::future<void>            future;
};

class WebApp : public Web {
public:
    WebApp();
    virtual ~WebApp() override;
};
class WebManager : public Web {
public:
    WebManager();
};

inline std::atomic<bool> stop{false};
void                     run();

} // namespace web

} // namespace modules
} // namespace jx3calc

#endif // MODULES_WEB_H_
