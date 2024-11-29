//
// Created by mecha on 29.11.2024.
//

#include "Mutex.hpp"

void Mutex::init() {
    pthread_mutex_init(&handle, nullptr);
}

void Mutex::free() {
    pthread_mutex_destroy(&handle);
}

void Mutex::lock() {
    pthread_mutex_lock(&handle);
}

void Mutex::unlock() {
    pthread_mutex_unlock(&handle);
}
