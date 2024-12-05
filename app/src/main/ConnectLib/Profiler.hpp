//
// Created by mecha on 29.11.2024.
//

#ifndef CONNECT_PROFILER_HPP
#define CONNECT_PROFILER_HPP

#if defined(PROFILE_DEBUG)

#define PROFILE() Profiler profiler##__LINE__(__FILENAME__, __FUNCTION__, __LINE__)

#else

#define PROFILE()

#endif

struct Profile {
    const char* fileName;
    const char* functionName;
    int line;
    float beginTime;
    float endTime;
};

class Profiler {

public:
    Profiler(const char* fileName, const char* functionName, int line);
    ~Profiler();

private:
    Profile profile;
};

#endif //CONNECT_PROFILER_HPP