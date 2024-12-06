//
// Created by cheerwizard on 28.11.24.
//

#include "Logger.hpp"

Logger::Logger(const char* tag, const char* filepath)
:
threadPool(1, 10, "Logger", ThreadPriority_HIGHEST)
{
    ASSERT(tag, "Logger tag is NULL!");
    threadPool.push([=]() {
        file = fopen(filepath, "w");
        if (file == nullptr) {
            error(__FILE__, __FUNCTION__, __LINE__, "Failed to open log file %s", filepath);
        } else {
            info("Log file %s is open.", filepath);
        }
    });
}

Logger::~Logger() {
    LOG_INFO("Logger file is closing...");
    threadPool.push([=]() {
        fclose(file);
    });
}

void Logger::writeToFile(char* msg) {
    if (file != nullptr) {
        fputs(msg, file);
        fflush(file);
    }
}
