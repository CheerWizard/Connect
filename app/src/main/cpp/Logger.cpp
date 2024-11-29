//
// Created by cheerwizard on 28.11.24.
//

#include "Logger.hpp"

#include <android/set_abort_message.h>
#include <android/log.h>
#include <sys/mman.h>

#define TAG "Connect"

void Logger::open(const char *filepath) {
    threadPool = new ThreadPool(1, 10, "Log", ThreadPriority_HIGHEST);
    threadPool->push([=] {
        file = fopen(filepath, "rw");
        if (file == nullptr) {
            LOG_ERR("Failed to open log file %s", filepath);
        }
        LOG_INFO("Log file %s is open.", filepath);
    });
}

void Logger::close() {
    LOG_INFO("Logger file is closing...");
    threadPool->push([=]() {
        fclose(file);
        delete threadPool;
    });
}

void Logger::verboseInternal(LogColor color, char *log) {
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_VERBOSE, TAG, log);
}

void Logger::infoInternal(LogColor color, char *log) {
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_INFO, TAG, log);
}

void Logger::debugInternal(LogColor color, char *log) {
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_DEBUG, TAG, log);
}

void Logger::warningInternal(LogColor color, char *log) {
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_WARN, TAG, log);
}

void Logger::errorInternal(LogColor color, char *log) {
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_ERROR, TAG, log);
}

void Logger::assertInternal(LogColor color, char *log) {
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_FATAL, TAG, log);
}

void Logger::abortInternal(LogColor color, char *log) {
    android_set_abort_message(log);
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_FATAL, TAG, log);
    std::abort();
}