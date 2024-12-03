//
// Created by cheerwizard on 02.12.24.
//

#ifndef CONNECT_AUDIOBUFFERQUEUE_HPP
#define CONNECT_AUDIOBUFFERQUEUE_HPP

#include "BufferQueue.hpp"

struct SampleBuffer {
    uint8_t* buffer;   // audio sample container
    uint32_t capacity;   // buffer capacity in byte
    uint32_t size;  // audio sample size (n buf) in byte
};

using AudioBufferQueue = BufferQueue<SampleBuffer*>;

__inline__ void releaseSampleBufs(SampleBuffer* bufs, uint32_t& count) {
    if (!bufs || !count) {
        return;
    }
    for (uint32_t i = 0; i < count; i++) {
        if (bufs[i].buffer) delete[] bufs[i].buffer;
    }
    delete[] bufs;
}

__inline__ SampleBuffer* allocateSampleBuffer(uint32_t count, uint32_t sizeInByte) {
    if (count <= 0 || sizeInByte <= 0) {
        return nullptr;
    }

    SampleBuffer* bufs = new SampleBuffer[count];
    ASSERT(bufs, "Sample Buffer is NULL!");
    memset(bufs, 0, sizeof(SampleBuffer) * count);

    uint32_t allocSize = (sizeInByte + 3) & ~3;  // padding to 4 bytes aligned
    uint32_t i;

    for (i = 0; i < count; i++) {
        bufs[i].buffer = new uint8_t[allocSize];
        if (bufs[i].buffer == nullptr) {
            LOG_WARN("Requesting %d buffers, allocated %d", count, i);
            break;
        }
        bufs[i].capacity = sizeInByte;
        bufs[i].size = 0;  // 0 data in it
    }

    if (i < 2) {
        releaseSampleBufs(bufs, i);
        bufs = nullptr;
    }

    count = i;
    return bufs;
}

#endif //CONNECT_AUDIOBUFFERQUEUE_HPP