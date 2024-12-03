//
// Created by mecha on 29.11.2024.
//

#include "Thread.hpp"

static const int ThreadPriorities[ThreadPriority_COUNT] = {
        1, // ThreadPriority_LOWEST
        2, // ThreadPriority_NORMAL
        3, // ThreadPriority_HIGHEST
};

Thread::Thread(const char* name, ThreadPriority priority) {
    this->name = name;
    this->priority = priority;
    signal(SIGABRT, [](int) {
        pthread_exit(0);
    });
}

Thread::~Thread() {
    int result = pthread_kill(thread, SIGABRT);
    ASSERT(result == 0, "Unable to free a Thread=%s", name);
    signal(SIGABRT, SIG_DFL);
}

static void* threadRunFunction(void* thread) {
    Thread& t = *static_cast<Thread*>(thread);
#if defined(DEBUG)
    t.setInfo();
#endif
    t.runnable();
    return nullptr;
}

void Thread::run(const std::function<void()> &runnable) {
    this->runnable = runnable;
    int result = pthread_create(&thread, nullptr, threadRunFunction, this);
    ASSERT(result == 0, "Unable to create a Thread=%s", name);
}

void Thread::detach() {
    int result = pthread_detach(thread);
    ASSERT(result == 0, "Unable to detach a pthread %s", name);
}

void Thread::join() {
    int result = pthread_join(thread, nullptr);
    ASSERT(result == 0, "Unable to join to a pthread %s", name);
}

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

void Thread::sleep(uint32_t ms) {
    int result = usleep(ms * 1000);
    ASSERT(result == 0, "Unable to sleep this thread");
}

void Thread::yield() {
    int result = sched_yield();
    ASSERT(result == 0, "Unable to yield this thread");
}

void Thread::exit() {
    pthread_exit(0);
}