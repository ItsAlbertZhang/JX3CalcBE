#ifndef THREAD_WEB_HELPER_H_
#define THREAD_WEB_HELPER_H_

namespace ns_thread {

class WebHelper {
public:
    WebHelper()                             = default;
    virtual ~WebHelper()                    = default;
    WebHelper(const WebHelper &)            = delete;
    WebHelper &operator=(const WebHelper &) = delete;
    WebHelper(WebHelper &&)                 = delete;
    WebHelper &operator=(WebHelper &&)      = delete;
};

} // namespace ns_thread

#endif // THREAD_WEB_HELPER_H_
