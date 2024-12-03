//
// Created by cheerwizard on 28.11.24.
//

#ifndef CONNECT_LOGGER_HPP
#define CONNECT_LOGGER_HPP

#include "ThreadPool.hpp"
#include "Clock.hpp"

#if defined(DEBUG)

#define LOG_OPEN(tag, filepath) logger.open(tag, filepath)
#define LOG_CLOSE() logger.close()

#if defined(ANDROID)

#define LOG_VERB(msg, ...) logger.threadPool->push([=] { logger.verbose(msg, 0, ##__VA_ARGS__); })
#define LOG_INFO(msg, ...) logger.threadPool->push([=] { logger.info(msg, 0, ##__VA_ARGS__); })
#define LOG_DBG(msg, ...) logger.threadPool->push([=] { logger.debug(msg, 0, ##__VA_ARGS__); })
#define LOG_WARN(msg, ...) logger.threadPool->push([=] { logger.warning(msg, 0, ##__VA_ARGS__); })
#define LOG_ERR(msg, ...) logger.threadPool->push([=] { logger.error(__FILE__, __FUNCTION__, __LINE__, msg, 0, ##__VA_ARGS__); })
#define LOG_ASSERT(x, msg, ...) logger.threadPool->push([=] { logger.assertion(__FILE__, __FUNCTION__, __LINE__, msg, 0, ##__VA_ARGS__); })
#define LOG_ABORT(msg, ...) logger.threadPool->push([=] { logger.abort(__FILE__, __FUNCTION__, __LINE__, msg, 0, ##__VA_ARGS__); })

#else

#define LOG_VERB(msg, ...) logger.threadPool->push([=] { logger.verbose(msg, ##__VA_ARGS__); })
#define LOG_INFO(msg, ...) logger.threadPool->push([=] { logger.info(msg, ##__VA_ARGS__); })
#define LOG_DBG(msg, ...) logger.threadPool->push([=] { logger.debug(msg, ##__VA_ARGS__); })
#define LOG_WARN(msg, ...) logger.threadPool->push([=] { logger.warning(msg, ##__VA_ARGS__); })
#define LOG_ERR(msg, ...) logger.threadPool->push([=] { logger.error(__FILE__, __FUNCTION__, __LINE__, msg, ##__VA_ARGS__); })
#define LOG_ASSERT(x, msg, ...) logger.threadPool->push([=] { logger.assertion(__FILE__, __FUNCTION__, __LINE__, msg, ##__VA_ARGS__); })
#define LOG_ABORT(msg, ...) logger.threadPool->push([=] { logger.abort(__FILE__, __FUNCTION__, __LINE__, msg, ##__VA_ARGS__); })

#endif

#else

#define LOG_OPEN(filepath)
#define LOG_CLOSE()

#define LOG_VERB(msg, ...)
#define LOG_INFO(msg, ...)
#define LOG_DBG(msg, ...)
#define LOG_WARN(msg, ...)
#define LOG_ERR(msg, ...)
#define LOG_ASSERT(x, msg, ...)
#define LOG_ABORT(msg, ...)

#endif

enum LogColor {
    LogColor_BLACK,
    LogColor_BLUE,
    LogColor_GREEN,
    LogColor_AQUA,
    LogColor_RED,
    LogColor_PURPLE,
    LogColor_YELLOW,
    LogColor_WHITE,
    LogColor_GRAY,
    LogColor_LIGHT_BLUE,
    LogColor_LIGHT_GREEN,
    LogColor_LIGHT_AQUA,
    LogColor_LIGHT_RED,
    LogColor_LIGHT_PURPLE,
    LogColor_LIGHT_YELLOW,
    LogColor_LIGHT_WHITE,

    LogColor_COUNT
};

class Logger {

public:
    ThreadPool* threadPool = nullptr;

    void open(const char* tag, const char* filepath);

    void close();

    template<typename... Args>
    void verbose(const char* msg, Args &&... args);

    template<typename... Args>
    void info(const char* msg, Args &&... args);

    template<typename... Args>
    void debug(const char* msg, Args &&... args);

    template<typename... Args>
    void warning(const char* msg, Args &&... args);

    template<typename... Args>
    void error(const char* filename, const char* function, int line, const char* msg, Args &&... args);

    template<typename... Args>
    void assertion(const char* filename, const char* function, int line, const char* msg, Args &&... args);

    template<typename... Args>
    void abort(const char* filename, const char* function, int line, const char* msg, Args &&... args);

private:
    void printVerbose(LogColor color, char* log);
    void printInfo(LogColor color, char* log);
    void printDebug(LogColor color, char* log);
    void printWarning(LogColor color, char* log);
    void printError(LogColor color, char* log);
    void printAssert(LogColor color, char* log);
    void printAbort(LogColor color, char* log);

    FILE* file = nullptr;
    const char* tag = nullptr;
    char buffer[256] = {};
};

inline Logger logger = {};

template<typename ... Args>
void Logger::verbose(const char* msg, Args &&... args) {
    // TODO: consider how to copy msg and args into thread pool tasks
    sprintf(buffer, msg, args...);
    threadPool->push([=]() {
        char fmt_buffer[256] = {};
        char text_buffer[256] = {};
        DateTime date_time = Clock::getDateTime();
#if defined(__LP64__)
        const char* fmt = "\n[%d.%d.%d][%ld:%ld:%ld.%ld][VERBOSE] %s";
#else
        const char* fmt = "\n[%d.%d.%d][%lld:%lld:%lld.%lld][VERBOSE] %s";
#endif
        sprintf(
                fmt_buffer,
                fmt,
                date_time.d, date_time.m, date_time.y, date_time.h, date_time.min, date_time.s, date_time.ms,
                msg
        );
        sprintf(text_buffer, fmt_buffer, buffer);
        printVerbose(LogColor_LIGHT_GREEN, text_buffer);
        if (file != nullptr) {
            fputs(text_buffer, file);
            fflush(file);
        }
    });
}

template<typename ... Args>
void Logger::info(const char* msg, Args &&... args) {
    char fmt_buffer[256] = {};
    char text_buffer[256] = {};
    DateTime date_time = Clock::getDateTime();
#if defined(__LP64__)
    const char* fmt = "\n[%d.%d.%d][%ld:%ld:%ld.%ld][INFO] %s";
#else
    const char* fmt = "\n[%d.%d.%d][%lld:%lld:%lld.%lld][INFO] %s";
#endif
    sprintf(
            fmt_buffer,
            fmt,
            date_time.d, date_time.m, date_time.y, date_time.h, date_time.min, date_time.s, date_time.ms,
            msg
    );
    sprintf(text_buffer, fmt_buffer, args...);
    printInfo(LogColor_GREEN, text_buffer);
    if (file != nullptr) {
        fputs(text_buffer, file);
        fflush(file);
    }
}

template<typename ... Args>
void Logger::debug(const char* msg, Args &&... args) {
    char fmt_buffer[256] = {};
    char text_buffer[256] = {};
    DateTime date_time = Clock::getDateTime();
#if defined(__LP64__)
    const char* fmt = "\n[%d.%d.%d][%ld:%ld:%ld.%ld][DEBUG] %s";
#else
    const char* fmt = "\n[%d.%d.%d][%lld:%lld:%lld.%lld][DEBUG] %s";
#endif
    sprintf(
            fmt_buffer,
            fmt,
            date_time.d, date_time.m, date_time.y, date_time.h, date_time.min, date_time.s, date_time.ms,
            msg
    );
    sprintf(text_buffer, fmt_buffer, args...);
    printDebug(LogColor_WHITE, text_buffer);
    if (file != nullptr) {
        fputs(text_buffer, file);
        fflush(file);
    }
}

template<typename ... Args>
void Logger::warning(const char* msg, Args &&... args) {
    char fmt_buffer[256] = {};
    char text_buffer[256] = {};
    DateTime date_time = Clock::getDateTime();
#if defined(__LP64__)
    const char* fmt = "\n[%d.%d.%d][%ld:%ld:%ld.%ld][WARNING] %s";
#else
    const char* fmt = "\n[%d.%d.%d][%lld:%lld:%lld.%lld][WARNING] %s";
#endif
    sprintf(
            fmt_buffer,
            fmt,
            date_time.d, date_time.m, date_time.y, date_time.h, date_time.min, date_time.s, date_time.ms,
            msg
    );
    sprintf(text_buffer, fmt_buffer, args...);
    printWarning(LogColor_YELLOW, text_buffer);
    if (file != nullptr) {
        fputs(text_buffer, file);
        fflush(file);
    }
}

template<typename ... Args>
void Logger::error(const char* filename, const char* function, int line, const char* msg, Args &&... args) {
    char fmt_buffer[256] = {};
    char text_buffer[256] = {};
    DateTime date_time = Clock::getDateTime();
#if defined(__LP64__)
    const char* fmt = "\n[%d.%d.%d][%ld:%ld:%ld.%ld][ERROR] log_error in %s -> %s(%i line):\n%s";
#else
    const char* fmt = "\n[%d.%d.%d][%lld:%lld:%lld.%lld][ERROR] Error in %s -> %s(%i line):\n%s";
#endif
    sprintf(
            fmt_buffer,
            fmt,
            date_time.d, date_time.m, date_time.y, date_time.h, date_time.min, date_time.s, date_time.ms,
            filename, function, line,
            msg
    );
    sprintf(text_buffer, fmt_buffer, args...);
    printError(LogColor_RED, text_buffer);
    if (file != nullptr) {
        fputs(text_buffer, file);
        fflush(file);
    }
}

template<typename ... Args>
void Logger::assertion(const char* filename, const char* function, int line, const char* msg, Args &&... args) {
    char fmt_buffer[256] = {};
    char text_buffer[256] = {};
    DateTime date_time = Clock::getDateTime();
#if defined(__LP64__)
    const char* fmt = "\n[%d.%d.%d][%ld:%ld:%ld.%ld][ASSERT] Assertion Failed in %s -> %s(%i line):\n%s";
#else
    const char *fmt = "\n[%d.%d.%d][%lld:%lld:%lld.%lld][ASSERT] Assertion Failed in %s -> %s(%i line):\n%s";
#endif
    sprintf(
            fmt_buffer,
            fmt,
            date_time.d, date_time.m, date_time.y, date_time.h, date_time.min, date_time.s, date_time.ms,
            filename, function, line,
            msg
    );
    sprintf(text_buffer, fmt_buffer, args...);
    printAssert(LogColor_RED, text_buffer);
    if (file != nullptr) {
        fputs(text_buffer, file);
        fflush(file);
    }
}

template<typename ... Args>
void Logger::abort(const char* filename, const char* function, int line, const char* msg, Args &&... args) {
    char fmt_buffer[256] = {};
    char text_buffer[256] = {};
    DateTime date_time = Clock::getDateTime();
#if defined(__LP64__)
    const char* fmt = "\n[%d.%d.%d][%ld:%ld:%ld.%ld][ABORT] Abortion in %s -> %s(%i line):\n%s";
#else
    const char *fmt = "\n[%d.%d.%d][%lld:%lld:%lld.%lld][ABORT] Abortion in %s -> %s(%i line):\n%s";
#endif
    sprintf(
            fmt_buffer,
            fmt,
            date_time.d, date_time.m, date_time.y, date_time.h, date_time.min, date_time.s, date_time.ms,
            filename, function, line,
            msg
    );
    sprintf(text_buffer, fmt_buffer, args...);
    printAbort(LogColor_RED, text_buffer);
    if (file != nullptr) {
        fputs(text_buffer, file);
        fflush(file);
    }
}

#endif //CONNECT_LOGGER_HPP