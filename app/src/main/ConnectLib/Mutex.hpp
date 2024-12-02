//
// Created by mecha on 29.11.2024.
//

#ifndef CONNECT_MUTEX_HPP
#define CONNECT_MUTEX_HPP

#include "Types.hpp"

class Mutex {

public:
    void init();
    void free();

    void lock();
    void unlock();

    inline pthread_mutex_t& getHandle() { return handle; }

private:
    pthread_mutex_t handle = {};
};

#endif //CONNECT_MUTEX_HPP
