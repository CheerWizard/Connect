//
// Created by mecha on 29.11.2024.
//

#include "Mutex.hpp"

Mutex::Mutex() {
    pthread_mutex_init(&handle, nullptr);
}

Mutex::~Mutex() {
    pthread_mutex_destroy(&handle);
}

void Mutex::lock() {
    pthread_mutex_lock(&handle);
}

void Mutex::unlock() {
    pthread_mutex_unlock(&handle);
}

LockGuard::LockGuard(Mutex& mutex) : mutex(mutex) {
    this->mutex.lock();
}

LockGuard::~LockGuard() {
    this->mutex.unlock();
}