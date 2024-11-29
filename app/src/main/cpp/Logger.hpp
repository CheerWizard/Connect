//
// Created by cheerwizard on 28.11.24.
//

#ifndef CONNECT_LOGGER_HPP
#define CONNECT_LOGGER_HPP

#include "Types.hpp"

#define CHECK_NOT_NULL(value)                                           \
  do {                                                                  \
    if ((value) == nullptr) {                                           \
      fatal("%s:%d:%s must not be null", __PRETTY_FUNCTION__, __LINE__, \
            #value);                                                    \
    }                                                                   \
  } while (false)                                                       \


#if defined(DEBUG)

#define LOG_OPEN(filepath) global_log.open(filepath)
#define LOG_CLOSE() global_log.close()

#if defined(ANDROID)

#define LOG_VERB(msg, ...) global_log.verbose(msg, 0, ##__VA_ARGS__)
#define LOG_INFO(msg, ...) global_log.info(msg, 0, ##__VA_ARGS__)
#define LOG_DBG(msg, ...) global_log.debug(msg, 0, ##__VA_ARGS__)
#define LOG_WARN(msg, ...) global_log.warning(msg, 0, ##__VA_ARGS__)
#define LOG_ERR(msg, ...) global_log.error(__FILE__, __FUNCTION__, __LINE__, msg, 0, ##__VA_ARGS__)
#define LOG_ASSERT(x, msg, ...) global_log.assert(__FILE__, __FUNCTION__, __LINE__, msg, 0, ##__VA_ARGS__)

#else

#define LOG_VERB(msg, ...) global_log.verbose(msg, ##__VA_ARGS__)
#define LOG_INFO(msg, ...) global_log.info(msg, ##__VA_ARGS__)
#define LOG_DBG(msg, ...) global_log.debug(msg, ##__VA_ARGS__)
#define LOG_WARN(msg, ...) global_log.warning(msg, ##__VA_ARGS__)
#define LOG_ERR(msg, ...) global_log.error(__FILE__, __FUNCTION__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_ASSERT(x, msg, ...) global_log.assert(__FILE__, __FUNCTION__, __LINE__, msg, ##__VA_ARGS__)

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

#endif

enum LogColor {
    BLACK,
    BLUE,
    GREEN,
    AQUA,
    RED,
    PURPLE,
    YELLOW,
    WHITE,
    GRAY,
    LIGHT_BLUE,
    LIGHT_GREEN,
    LIGHT_AQUA,
    LIGHT_RED,
    LIGHT_PURPLE,
    LIGHT_YELLOW,
    LIGHT_WHITE,

    COUNT
};

class Logger {

    void open(const char* filepath);

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
    void verboseInternal(LogColor color, char* log);
    void infoInternal(LogColor color, char* log);
    void debugInternal(LogColor color, char* log);
    void warningInternal(LogColor color, char* log);
    void errorInternal(LogColor color, char* log);
    void assertInternal(LogColor color, char* log);
    void abortInternal(LogColor color, char* log);

    File* file = nullptr;
    ThreadPool* threadPool = nullptr;
};

inline Logger logger = {};

template<typename ... Args>
void Logger::verbose(const char* msg, Args &&... args) {
    thread_pool->push([=] {
        char fmt_buffer[256] = {};
        char text_buffer[256] = {};
        date_time_t date_time = clock_t().get_current_date_time();
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
        sprintf(text_buffer, fmt_buffer, args...);
        verboseInternal(LogColor::LIGHT_GREEN, text_buffer);
        if (file.is_open()) {
            file.write(text_buffer, 256);
        }
    });
}

template<typename ... Args>
void Logger::info(const char* msg, Args &&... args) {
    thread_pool->push([=] {
        char fmt_buffer[256] = {};
        char text_buffer[256] = {};
        date_time_t date_time = clock_t().get_current_date_time();
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
        console_info(GREEN, text_buffer);
        if (file.is_open()) {
            file.write(text_buffer, 256);
        }
    });
}

template<typename ... Args>
void Logger::debug(const char* msg, Args &&... args) {
    thread_pool->push([=] {
        char fmt_buffer[256] = {};
        char text_buffer[256] = {};
        date_time_t date_time = clock_t().get_current_date_time();
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
        console_debug(WHITE, text_buffer);
        if (file.is_open()) {
            file.write(text_buffer, 256);
        }
    });
}

template<typename ... Args>
void Logger::warning(const char* msg, Args &&... args) {
    thread_pool->push([=] {
        char fmt_buffer[256] = {};
        char text_buffer[256] = {};
        date_time_t date_time = clock_t().get_current_date_time();
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
        console_warning(YELLOW, text_buffer);
        if (file.is_open()) {
            file.write(text_buffer, 256);
        }
    });
}

template<typename ... Args>
void Logger::error(const char* filename, const char* function, int line, const char* msg, Args &&... args) {
    thread_pool->push([=] {
        char fmt_buffer[256] = {};
        char text_buffer[256] = {};
        date_time_t date_time = clock_t().get_current_date_time();
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
        console_error(RED, text_buffer);
        if (file.is_open()) {
            file.write(text_buffer, 256);
        }
    });
}

template<typename ... Args>
void Logger::assert(const char* filename, const char* function, int line, const char* msg, Args &&... args) {
        thread_pool->push([=] {
            char fmt_buffer[256] = {};
            char text_buffer[256] = {};
            date_time_t date_time = clock_t().get_current_date_time();
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
            console_assert(RED, text_buffer);
            if (file.is_open()) {
                file.write(text_buffer, 256);
            }
        });
}

#endif //CONNECT_LOGGER_HPP