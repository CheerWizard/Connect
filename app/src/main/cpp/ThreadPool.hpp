//
// Created by mecha on 29.11.2024.
//

#ifndef CONNECT_THREADPOOL_HPP
#define CONNECT_THREADPOOL_HPP

#include "Thread.hpp"
#include "ConditionVar.hpp"
#include "CircleBuffer.hpp"

class ThreadPool {

public:
    ThreadPool(size_t size, size_t taskSize, const char* name, ThreadPriority priority);
    ~ThreadPool();

    void push(const function<void()>& task);

private:
    void run();

    bool running = false;
    vector<Thread*> threads;
    CircleBuffer<function<void()>> tasks;
    Mutex wakeMutex;
    ConditionVar wakeConditionVar;
};

#endif //CONNECT_THREADPOOL_HPP
