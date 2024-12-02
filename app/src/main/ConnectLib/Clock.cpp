//
// Created by mecha on 29.11.2024.
//

#include "Clock.hpp"

DateTime Clock::getDateTime() {
    const ::time_t t = ::time(nullptr);
    const ::tm* lt = ::localtime(&t);
    DateTime dateTime = {};
    dateTime.y = lt->tm_year;
    dateTime.m = lt->tm_mon;
    dateTime.d = lt->tm_mday;
    dateTime.h = lt->tm_hour;
    dateTime.min = lt->tm_min;
    dateTime.s = lt->tm_sec;
    dateTime.ms = lt->tm_sec * 1000;
    return dateTime;
}

Time Clock::getTime() {
    const ::time_t t = ::time(nullptr);
    const ::tm* lt = ::localtime(&t);
    Time time = {};
    time.ms = lt->tm_sec * 1000;
    time.s = lt->tm_sec;
    time.min = lt->tm_min;
    time.h = lt->tm_hour;
    return time;
}

float Clock::getTimeMillis() {
    const ::time_t t = ::time(nullptr);
    const ::tm* lt = ::localtime(&t);
    return lt->tm_sec * 1000;
}