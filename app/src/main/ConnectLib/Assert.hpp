//
// Created by cheerwizard on 29.11.24.
//

#ifndef CONNECT_ASSERT_HPP
#define CONNECT_ASSERT_HPP

#include "DebugBreak.hpp"

template<typename... Args>
void assertInternal(bool condition, const char* conditionStr, const char* msg, Args ... args);

#if defined(DEBUG)

#if defined(ANDROID)

#define ASSERT(condition, msg, ...) assertInternal(condition, #condition, msg, 0, ##__VA_ARGS__)

#else

#define ASSERT(condition, msg, ...) assertInternal(condition, #condition, msg, ##__VA_ARGS__)

#endif

#else

#define ASSERT(condition, msg, ...)

#endif

#if defined(ANDROID)

#include <android/log.h>

template<typename... Args>
void assertInternal(bool condition, const char* conditionStr, const char* msg, Args ... args) {
    if (!condition) {
        char buffer[256] = {};
        sprintf(buffer, msg, args...);
        __android_log_assert(conditionStr, "ASSERT", buffer);
    }
}

#else

template<typename... Args>
void assertInternal(bool condition, const char* conditionStr, const char* msg, Args ... args) {
    if (!condition) {
        char buffer[256] = {};
        sprintf(buffer, msg, args...);
        DEBUG_BREAK();
    }
}

#endif

#endif //CONNECT_ASSERT_HPP
