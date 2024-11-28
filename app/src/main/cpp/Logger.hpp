//
// Created by cheerwizard on 28.11.24.
//

#ifndef CONNECT_LOGGER_HPP
#define CONNECT_LOGGER_HPP

#include "Types.hpp"
#include <android/set_abort_message.h>
#include <android/log.h>

#define LOG(priority, fmt, ...) \
  ((void)__android_log_print((priority), __FILE_NAME__, (fmt)__VA_OPT__(, ) __VA_ARGS__))

#define LOGE(fmt, ...) LOG(ANDROID_LOG_ERROR, (fmt)__VA_OPT__(, ) __VA_ARGS__)
#define LOGW(fmt, ...) LOG(ANDROID_LOG_WARN, (fmt)__VA_OPT__(, ) __VA_ARGS__)
#define LOGI(fmt, ...) LOG(ANDROID_LOG_INFO, (fmt)__VA_OPT__(, ) __VA_ARGS__)

[[noreturn]] __attribute__((__format__(__printf__, 1, 2))) inline void fatal(
        const char* fmt, ...
) {
    va_list ap;
    va_start(ap, fmt);
    char* buf;
    if (vasprintf(&buf, fmt, ap) < 0) {
        android_set_abort_message("Failed for format error message");
    } else {
        android_set_abort_message(buf);
        // Also log directly, since the default Android Studio logcat filter hides
        // the backtrace which would otherwise show the abort message.
        LOGE("%s", buf);
    }
    std::abort();
}

#define CHECK_NOT_NULL(value)                                           \
  do {                                                                  \
    if ((value) == nullptr) {                                           \
      fatal("%s:%d:%s must not be null", __PRETTY_FUNCTION__, __LINE__, \
            #value);                                                    \
    }                                                                   \
  } while (false)


#endif //CONNECT_LOGGER_HPP
