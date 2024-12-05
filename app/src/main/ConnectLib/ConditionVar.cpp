//
// Created by mecha on 29.11.2024.
//

#include "ConditionVar.hpp"

ConditionVar::ConditionVar() {
    pthread_cond_init(&handle, nullptr);
}

ConditionVar::~ConditionVar() {
    pthread_cond_destroy(&handle);
}

void ConditionVar::wait(Mutex &mutex) {
    mutex.lock();
    int result = pthread_cond_wait(&handle, &mutex.getHandle());
    ASSERT(result == 0, "Unable to wait a pthread with condition var");
}

void ConditionVar::notify() {
    int result = pthread_cond_signal(&handle);
    ASSERT(result == 0, "Unable to notify a pthread with condition var");
}