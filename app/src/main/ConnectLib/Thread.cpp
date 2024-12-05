//
// Created by mecha on 29.11.2024.
//

#include "Thread.hpp"

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
    ASSERT(result == 0, "Unable to init a Thread=%s", name);
}

void Thread::detach() {
    int result = pthread_detach(thread);
    ASSERT(result == 0, "Unable to detach a pthread %s", name);
}

void Thread::join() {
    int result = pthread_join(thread, nullptr);
    ASSERT(result == 0, "Unable to join to a pthread %s", name);
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