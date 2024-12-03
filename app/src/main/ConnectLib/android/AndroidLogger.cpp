//
// Created by cheerwizard on 30.11.24.
//

#include "Logger.hpp"

void Logger::printVerbose(LogColor color, char *log) {
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_VERBOSE, tag, log);
}

void Logger::printInfo(LogColor color, char *log) {
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_INFO, tag, log);
}

void Logger::printDebug(LogColor color, char *log) {
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_DEBUG, tag, log);
}

void Logger::printWarning(LogColor color, char *log) {
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_WARN, tag, log);
}

void Logger::printError(LogColor color, char *log) {
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_ERROR, tag, log);
}

void Logger::printAssert(LogColor color, char *log) {
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_FATAL, tag, log);
}

void Logger::printAbort(LogColor color, char *log) {
    android_set_abort_message(log);
    __android_log_buf_write(LOG_ID_MAIN, ANDROID_LOG_FATAL, tag, log);
    std::abort();
}