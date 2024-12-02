//
// Created by mecha on 29.11.2024.
//

#ifndef CONNECT_THREAD_HPP
#define CONNECT_THREAD_HPP

#include "Types.hpp"

enum ThreadPriority {
    ThreadPriority_LOWEST,
    ThreadPriority_NORMAL,
    ThreadPriority_HIGHEST,

    ThreadPriority_COUNT
};

class Thread {

public:
    function<void()> runnable;

    Thread(const char* name, ThreadPriority priority);
    ~Thread();

    void run(const function<void()>& runnable);
    void detach();
    void join();
    void setInfo();
    static void sleep(uint32_t ms);
    static void yield();
    static void exit();

private:
    pthread_t thread;
    pid_t pid = 0;
    pid_t tid = 0;
    const char* name = nullptr;
    ThreadPriority priority = ThreadPriority_NORMAL;
};

#endif //CONNECT_THREAD_HPP
