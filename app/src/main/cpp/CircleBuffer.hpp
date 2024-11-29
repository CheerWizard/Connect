//
// Created by mecha on 29.11.2024.
//

#ifndef CONNECT_CIRCLEBUFFER_HPP
#define CONNECT_CIRCLEBUFFER_HPP

#include "Types.hpp"

template<typename T>
struct CircleBuffer final {
    friend CircleBuffer;

    inline T& operator[](int i) { return items[i]; }
    inline const T& operator[](int i) const { return items[i]; }
    inline const size_t getSize() { return size; }

    void resize(size_t size);
    bool push(const T& item);
    bool pop(T& item);

private:
    vector<T> items;
    size_t size = 0;
    size_t tail = 0;
    size_t head = 0;
};

template<typename T>
void CircleBuffer<T>::resize(size_t newSize) {
    items.resize(newSize);
}

template<typename T>
bool CircleBuffer<T>::push(const T &item) {
    bool pushed = false;

    size_t next = (head + 1) % size;
    if (next != tail) {
        items[head] = item;
        head = next;
        pushed = true;
    }

    return pushed;
}

template<typename T>
bool CircleBuffer<T>::pop(T &item) {
    bool popped = false;

    if (tail != head) {
        item = items[tail];
        tail = (tail + 1) % size;
        popped = true;
    }

    return popped;
}

#endif //CONNECT_CIRCLEBUFFER_HPP