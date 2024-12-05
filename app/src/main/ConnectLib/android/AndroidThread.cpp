//
// Created by cheerwizard on 05.12.24.
//

#include "Thread.hpp"

static const int ThreadPriorities[ThreadPriority_COUNT] = {
        1, // ThreadPriority_LOWEST
        2, // ThreadPriority_NORMAL
        3, // ThreadPriority_HIGHEST
};

void Thread::setInfo() {
    pid = getpid();
    tid = pthread_self();

    int result;

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    size_t cpusetsize = sizeof(cpuset);
    CPU_SET(tid, &cpuset);

    // Set affinity mask
    result = sched_setaffinity(pid, cpusetsize, &cpuset);
    ASSERT(result != 0, "Failed to set thread affinity mask on Android!");

    // Set priority
    sched_param param {};
    param.sched_priority = ThreadPriorities[priority];
    result = pthread_setschedparam(thread, SCHED_OTHER, &param);
    ASSERT(result != 0, "Failed to set thread priority on Android!");

    // Set name
//    result = pthread_setname_np(thread, name);
//    ASSERT(result != 0, "Failed to set thread name on Android!");
}