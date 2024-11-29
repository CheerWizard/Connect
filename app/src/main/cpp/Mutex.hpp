//
// Created by mecha on 29.11.2024.
//

#ifndef CONNECT_MUTEX_HPP
#define CONNECT_MUTEX_HPP

#include <Types.hpp>

class Mutex {

public:
    Mutex();
    ~Mutex();

private:
    pthread_mutex_t mutex = {};
};

#endif //CONNECT_MUTEX_HPP
