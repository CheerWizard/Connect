//
// Created by cheerwizard on 02.12.24.
//

#ifndef CONNECT_BUFFERQUEUE_HPP
#define CONNECT_BUFFERQUEUE_HPP

#ifndef CACHE_ALIGN
#define CACHE_ALIGN 64
#endif

template <typename T>
class BufferQueue {
public:
    explicit BufferQueue(int size) : BufferQueue(size, new T[size]) {}

    explicit BufferQueue(int size, T* buffer) : size(size), buffer(buffer) {
        // This is necessary because we depend on twos-complement wraparound
        // to take care of overflow conditions.
        ASSERT(size < numeric_limits<int>::max(), "BufferQueue buffer overflow with size %i", size);
    }

    bool push(const T& item) {
        return push([&](T* ptr) -> bool {
            *ptr = item;
            return true;
        });
    }

    // get() is idempotent between calls to commit().
    T* getWriteablePtr() {
        T* result = nullptr;

        bool check __attribute__((unused));  //= false;

        check = push([&](T* head) -> bool {
            result = head;
            return false;  // don't increment
        });

        // if there's no space, result should not have been set, and vice versa
        assert(check == (result != nullptr));

        return result;
    }

    bool commitWriteablePtr(T* ptr) {
        bool result = push([&](T* head) -> bool {
            // this writer func does nothing, because we assume that the caller
            // has already written to *ptr after acquiring it from a call to get().
            // So just double-check that ptr is actually at the write head, and
            // return true to indicate that it's safe to advance.

            // if this isn't the same pointer we got from a call to get(), then
            // something has gone terribly wrong. Either there was an intervening
            // call to push() or commit(), or the pointer is spurious.
            assert(ptr == head);
            return true;
        });
        return result;
    }

    // writer() can return false, which indicates that the caller
    // of push() changed its mind while writing (e.g. ran out of bytes)
    template <typename F>
    bool push(const F& writer) {
        bool result = false;
        int readPtr = readAtomic.load(memory_order_acquire);
        int writePtr = writeAtomic.load(memory_order_relaxed);

        // note that while readPtr and writePtr will eventually
        // wrap around, taking their difference is still valid as
        // long as size < MAXINT.
        int space = size - (int)(writePtr - readPtr);
        if (space >= 1) {
            result = true;

            // writer
            if (writer(buffer.get() + (writePtr % size))) {
                ++writePtr;
                writeAtomic.store(writePtr, memory_order_release);
            }
        }
        return result;
    }

    // front out the queue, but not pop-out
    bool front(T* out_item) {
        return front([&](T* ptr) -> bool {
            *out_item = *ptr;
            return true;
        });
    }

    void pop(void) {
        int readPtr = readAtomic.load(memory_order_relaxed);
        ++readPtr;
        readAtomic.store(readPtr, memory_order_release);
    }

    template <typename F>
    bool front(const F& reader) {
        bool result = false;

        int writePtr = writeAtomic.load(memory_order_acquire);
        int readPtr = readAtomic.load(memory_order_relaxed);

        // As above, wraparound is ok
        int available = (int)(writePtr - readPtr);
        if (available >= 1) {
            result = true;
            reader(buffer.get() + (readPtr % size));
        }

        return result;
    }

    uint32_t getSize() {
        int writePtr = writeAtomic.load(memory_order_acquire);
        int readPtr = readAtomic.load(memory_order_relaxed);

        return (uint32_t)(writePtr - readPtr);
    }

private:
    int size;
    unique_ptr<T> buffer;

    // forcing cache line alignment to eliminate false sharing of the
    // frequently-updated read and write pointers. The object is to never
    // let these get into the "shared" state where they'd cause a cache miss
    // for every write.
    alignas(CACHE_ALIGN) atomic<int> readAtomic{0};
    alignas(CACHE_ALIGN) atomic<int> writeAtomic{0};
};

#endif //CONNECT_BUFFERQUEUE_HPP