//
// Created by mecha on 29.11.2024.
//

#ifndef CONNECT_MUTEX_HPP
#define CONNECT_MUTEX_HPP

class Mutex {

public:
    Mutex();
    ~Mutex();

    void lock();
    void unlock();

    inline pthread_mutex_t& getHandle() { return handle; }

private:
    pthread_mutex_t handle = {};
};

class LockGuard {

public:
    LockGuard(Mutex& mutex);
    ~LockGuard();

private:
    Mutex& mutex;

};

#define LOCK(mutex) LockGuard lock##__LINE__(mutex)

#endif //CONNECT_MUTEX_HPP