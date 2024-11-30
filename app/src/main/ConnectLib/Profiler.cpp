//
// Created by mecha on 29.11.2024.
//

#include "Profiler.hpp"
#include "Clock.hpp"
#include "Logger.hpp"

Profiler::Profiler(const char* fileName, const char* functionName, int line) {
    profile.fileName = fileName;
    profile.functionName = functionName;
    profile.line = line;
    profile.beginTime = Clock::getTimeMillis();
}

Profiler::~Profiler() {
    profile.endTime = Clock::getTimeMillis();
    LOG_INFO(
            "%s(%s:%s) -> tooks %f ms",
            profile.functionName,
            profile.fileName,
            profile.line,
            profile.endTime - profile.beginTime
    );
}