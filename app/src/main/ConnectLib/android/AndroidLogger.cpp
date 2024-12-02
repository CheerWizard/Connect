//
// Created by cheerwizard on 30.11.24.
//

#include "Logger.hpp"
#include "Android.hpp"

void Logger::printVerbose(LogColor color, char *log) {
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_VERBOSE, LOG_TAG, log);
}

void Logger::printInfo(LogColor color, char *log) {
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_INFO, LOG_TAG, log);
}

void Logger::printDebug(LogColor color, char *log) {
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_DEBUG, LOG_TAG, log);
}

void Logger::printWarning(LogColor color, char *log) {
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_WARN, LOG_TAG, log);
}

void Logger::printError(LogColor color, char *log) {
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_ERROR, LOG_TAG, log);
}

void Logger::printAssert(LogColor color, char *log) {
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_FATAL, LOG_TAG, log);
}

void Logger::printAbort(LogColor color, char *log) {
    android_set_abort_message(log);
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_FATAL, LOG_TAG, log);
    std::abort();
}