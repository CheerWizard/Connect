//
// Created by mecha on 29.11.2024.
//

#ifndef CONNECT_CLOCK_HPP
#define CONNECT_CLOCK_HPP

#include "Types.hpp"

struct DateTime {
    uint32_t y;   // year
    uint32_t m;   // month
    uint32_t d;   // day
    int64_t h;   // hour
    int64_t min; // minute
    int64_t s;   // second
    int64_t ms;  // millisecond
};

struct Time {
    int64_t h;   // hour
    int64_t min; // minute
    int64_t s;   // second
    int64_t ms;  // millisecond
};

class Clock {

public:
    static DateTime getDateTime();
    static Time getTime();
    static float getTimeMillis();
};

#endif //CONNECT_CLOCK_HPP
