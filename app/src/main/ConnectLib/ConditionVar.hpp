//
// Created by mecha on 29.11.2024.
//

#ifndef CONNECT_CONDITIONVAR_HPP
#define CONNECT_CONDITIONVAR_HPP

#include "Mutex.hpp"

class ConditionVar {

public:
    ConditionVar();
    ~ConditionVar();

    void wait(Mutex& mutex);
    void notify();

private:
    pthread_cond_t handle = {};
};

#endif //CONNECT_CONDITIONVAR_HPP
