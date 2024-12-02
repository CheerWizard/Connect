//
// Created by cheerwizard on 28.11.24.
//

#include "Logger.hpp"

void Logger::open(const char* filepath) {
    threadPool = new ThreadPool(1, 10, "Log", ThreadPriority_HIGHEST);
    threadPool->push([=]() {
        file = fopen(filepath, "w");
        if (file == nullptr) {
            error(__FILE__, __FUNCTION__, __LINE__, "Failed to open log file %s", filepath);
        } else {
            info("Log file %s is open.", filepath);
        }
    });
}

void Logger::close() {
    LOG_INFO("Logger file is closing...");
    threadPool->push([=]() {
        fclose(file);
        delete threadPool;
    });
}