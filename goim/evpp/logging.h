#pragma once

#include "evpp/platform_config.h"

namespace evpp {

    typedef enum {
        kLevelVerbose = 0,
        kLevelDebug = 1,
        kLevelInfo = 2,
        kLevelWarn = 3,
        kLevelError = 4,
    } TLogLevel;

    typedef void (*xlogger_appender_t)(const char* _log);
    xlogger_appender_t xlogger_SetAppender(xlogger_appender_t _appender);

    void xlogger(TLogLevel level, const char *fileName, const char *funcName, int lineNo,
                  const char* fmt, ...);
    void xlogger_SetLevel(TLogLevel _level);

#define __xlogger_c_impl(level, ...)    evpp::xlogger(level, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define XTRACE(...)               __xlogger_c_impl(evpp::kLevelVerbose, __VA_ARGS__)
#define XDEBUG(...)               __xlogger_c_impl(evpp::kLevelDebug, __VA_ARGS__)
#define XINFO(...)                __xlogger_c_impl(evpp::kLevelInfo, __VA_ARGS__)
#define XWARN(...)                __xlogger_c_impl(evpp::kLevelWarn, __VA_ARGS__)
#define XERROR(...)               __xlogger_c_impl(evpp::kLevelError, __VA_ARGS__)


//#define LOG_DEBUG std::cout << __FILE__ << ":" << __LINE__ << " "
//#define LOG_INFO  std::cout << __FILE__ << ":" << __LINE__ << " "
//#define LOG_ERROR std::cout << __FILE__ << ":" << __LINE__ << " "

}
