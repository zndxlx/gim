#include "evpp/inner_pre.h"

#include "evpp/libevent.h"

#ifdef H_OS_WINDOWS
#pragma comment(lib,"Ws2_32.lib")
#endif

#ifndef H_OS_WINDOWS
#include <signal.h>
#endif

#include <map>
#include <thread>
#include <mutex>

namespace evpp {

namespace {
struct OnStartup {
    OnStartup() {
#ifndef H_OS_WINDOWS
        if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
            XERROR("SIGPIPE set failed.");
            exit(-1);
        }
        XINFO("ignore SIGPIPE")
#endif
    }
    ~OnStartup() {
    }
} __s_onstartup;
}


#ifdef H_DEBUG_MODE
static std::map<struct event*, std::thread::id> evmap;
static std::mutex mutex;
#endif

int EventAdd(struct event* ev, const struct timeval* timeout) {
#ifdef H_DEBUG_MODE
    {
        std::lock_guard<std::mutex> guard(mutex);
        if (evmap.find(ev) == evmap.end()) {
            auto id = std::this_thread::get_id();
            evmap[ev] = id;
        } else {
            XERROR("Event %p, fd=%d event_add twice!", ev, ev->ev_fd);
            assert("event_add twice");
        }
    }

    XDEBUG("event_add ev=%p, fd=%d, user_ptr=%p, tid=%d", ev, ev->ev_fd, ev->ev_arg, std::this_thread::get_id());

    #endif
    return event_add(ev, timeout);
}

int EventDel(struct event* ev) {
#ifdef H_DEBUG_MODE
    {
        std::lock_guard<std::mutex> guard(mutex);
        auto it = evmap.find(ev);
        if (it == evmap.end()) {
            XERROR("Event %p, fd=%d not exist in event loop, maybe event_del twice.", ev, ev->ev_fd);
            assert("event_del twice");
        } else {
            auto id = std::this_thread::get_id();
            if (id != it->second) {
                XERROR("Event %p, fd=%d deleted in different thread.", ev, ev->ev_fd);
                assert(it->second == id);
            }
            evmap.erase(it);
        }
    }
    XDEBUG("event_del ev=%p, fd=%d, user_ptr=%p, tid=%d", ev, ev->ev_fd, ev->ev_arg, std::this_thread::get_id());
    #endif
    return event_del(ev);
}

int GetActiveEventCount() {
#ifdef H_DEBUG_MODE
    return evmap.size();
#else
    return 0;
#endif
}

}
