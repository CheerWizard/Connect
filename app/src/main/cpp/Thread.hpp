//
// Created by mecha on 29.11.2024.
//

#ifndef CONNECT_THREAD_HPP
#define CONNECT_THREAD_HPP

#include "Types.hpp"

enum ThreadPriority {
    LOWEST,
    NORMAL,
    HIGHEST,

    COUNT
};

class Thread {

public:
    Thread();
    Thread(const char* name, ThreadPriority priority);
    ~Thread();

    uint32_t getPid();
    uint32_t getTid();
    void run(const function<void()>& runnable);
    void detach();
    void join();
    void sleep(uint32_t ms);
    void yield();
    void exit();

private:
    void setInfo();

    pthread_t thread;
    function<void()> runnable;
    pid_t pid = 0;
    pid_t tid = 0;
    const char* name = nullptr;
    ThreadPriority priority = ThreadPriority::NORMAL;
};

#endif //CONNECT_THREAD_HPP
