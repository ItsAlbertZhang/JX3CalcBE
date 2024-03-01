#ifndef MODULES_WEB_H_
#define MODULES_WEB_H_

#include <future>
#pragma warning(push, 0)
#ifdef _WIN32
#include <sdkddkver.h>
#endif
#include <crow.h>
#include <crow/middlewares/cors.h>
#pragma warning(pop)

namespace ns_modules {

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

} // namespace ns_modules

#endif // MODULES_WEB_H_
