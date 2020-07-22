//
// Created by lixin on 2020/7/22.
//
#include "logging.h"
#include <iostream>
#include <stdarg.h>

namespace evpp {
    const char *LogTag = "IM";
    static xlogger_appender_t gs_appender = nullptr;
    static TLogLevel gs_level = kLevelDebug;
    static const char* levelStrings[] = {
            "V",
            "D",  // debug
            "I",  // info
            "W",  // warn
            "E",  // error
    };

    void xlogger_SetLevel(TLogLevel level){
        gs_level = level;
    }

    const char* ExtractFileName(const char* _path) {
        if (NULL == _path) return "";

        const char* pos = strrchr(_path, '\\');

        if (NULL == pos) {
            pos = strrchr(_path, '/');
        }

        if (NULL == pos || '\0' == *(pos + 1)) {
            return _path;
        } else {
            return pos + 1;
        }
    }


    void ExtractFunctionName(const char* _func, char* _func_ret, int _len) {
        if (NULL == _func)return;

        const char* start = _func;
        const char* end = NULL;
        const char* pos = _func;

        while ('\0' != *pos) {
            if (NULL == end && ' ' == *pos) {
                start = ++pos;
                continue;
            }

            if (':' == *pos && ':' == *(pos+1)) {
                pos += 2;
                start = pos;
                continue;
            }

            if ('(' == *pos) {
                end = pos;
            } else if (NULL != start && (':' == *pos || ']' == *pos)) {
                end = pos;
                break;
            }
            ++pos;
        }


        if (NULL == start || NULL == end || start + 1 >= end) {
            strncpy(_func_ret, _func, _len);
            _func_ret[_len - 1] = '\0';
            return;
        }

        ptrdiff_t len = end - start;
        --_len;
        len = _len < len ? _len : len;
        memcpy(_func_ret, start, len);
        _func_ret[len] = '\0';
    }


    xlogger_appender_t xlogger_SetAppender(xlogger_appender_t _appender) {
        xlogger_appender_t old_appender = gs_appender;
        gs_appender = _appender;
        return old_appender;
    }

    void defaultLog(const char* log){
        std::cout << log;
    }

    void xlogger(TLogLevel level, const char *fileName, const char *funcName, int lineNo, const char* fmt, ... ) {
        if (level < gs_level) {
            return;
        }

        char strFuncName[128] = { 0 };
        ExtractFunctionName(funcName, strFuncName, sizeof(strFuncName));
        const char* strfileName = ExtractFileName(fileName);

        va_list args;

        va_start(args,fmt);
        char temp[4096] = {'\0'};
        vsnprintf(temp, 4096, fmt, args);
        va_end(args);
        char log[8 * 1024] = { 0 };
        snprintf(log, sizeof(log), "[%s][%s][%s, %s, %d][%s]\n", levelStrings[level], LogTag, strfileName, strFuncName, lineNo, temp);

        if (gs_appender != nullptr) {
            gs_appender(log);
        }else{
            defaultLog(log);
        }
    }
}




