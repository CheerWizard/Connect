//
// Created by mecha on 29.11.2024.
//

#include "ThreadPool.hpp"

ThreadPool::ThreadPool(size_t threadSize, size_t taskSize, const char* name, ThreadPriority priority) {
    threads.resize(threadSize);
    tasks.resize(taskSize);
    wakeMutex.init();
    wakeConditionVar.init();
    for (int i = 0; i < threadSize ; i++) {
        threads[i] = new Thread(name, priority);
    }
    run();
}

ThreadPool::~ThreadPool() {
    running = false;
    const size_t thread_size = threads.size();
    for (Thread* thread : threads) {
        delete thread;
    }
    wakeMutex.free();
    wakeConditionVar.free();
}

void ThreadPool::push(const function<void()>& task) {
    // try to push a new task until it is pushed
    while (!tasks.push(task)) {
        wakeConditionVar.notify();
        Thread::yield();
    }
    wakeConditionVar.notify();
}

void ThreadPool::run() {
    if (running) {
        return;
    }

    running = true;
    const size_t thread_size = threads.size();
    for (int i = 0 ; i < thread_size ; i++) {
        Thread* thread = threads[i];
        thread->run([=] {
            while (running) {
                if (function<void()> task; tasks.pop(task)) {
                    task();
                }
                else {
                    // because we don't want to overhead cpu core with thread while loop
                    // it's better to simply put thread into wait, until it is notified by outer thread with wake condition variable
                    wakeConditionVar.wait(wakeMutex);
                }
            }
        });
        thread->detach();
    }
}