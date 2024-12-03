//
// Created by mecha on 29.11.2024.
//

#ifndef CONNECT_CIRCLEBUFFER_HPP
#define CONNECT_CIRCLEBUFFER_HPP

template<typename T>
struct CircleBuffer final {
    friend CircleBuffer;

    inline T& operator[](int i) { return items[i]; }
    inline const T& operator[](int i) const { return items[i]; }

    void reserve(size_t capacity);
    void resize(size_t size);
    bool push(const T& item);
    bool pop(T& item);

private:
    vector<T> items;
    size_t tail = 0;
    size_t head = 0;
};

template<typename T>
void CircleBuffer<T>::reserve(size_t newSize) {
    items.reserve(newSize);
}

template<typename T>
void CircleBuffer<T>::resize(size_t newSize) {
    items.resize(newSize);
}

template<typename T>
bool CircleBuffer<T>::push(const T &item) {
    bool pushed = false;

    size_t next = (head + 1) % items.size();
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
        tail = (tail + 1) % items.size();
        popped = true;
    }

    return popped;
}

#endif //CONNECT_CIRCLEBUFFER_HPP