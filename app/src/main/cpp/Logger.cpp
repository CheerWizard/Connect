//
// Created by cheerwizard on 28.11.24.
//

#include "Logger.hpp"

#include <android/set_abort_message.h>
#include <android/log.h>

va_list ap;
va_start(ap, fmt);
char* buf;
if (vasprintf(&buf, fmt, ap) < 0) {
android_set_abort_message("Failed for format error message");
} else {
android_set_abort_message(buf);
// Also log directly, since the default Android Studio logcat filter hides
// the backtrace which would otherwise show the abort message.
LOG_ERROR("%s", buf);
}
std::abort();