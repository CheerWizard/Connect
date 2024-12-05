//
// Created by cheerwizard on 02.12.24.
//

#ifndef CONNECT_AUDIO_HPP
#define CONNECT_AUDIO_HPP

/*
 * Audio Sample Controls...
 */
#define AUDIO_SAMPLE_CHANNELS 1

extern const uint32_t PCM_FORMAT_8;
extern const uint32_t PCM_FORMAT_16;
extern const uint32_t PCM_FORMAT_20;
extern const uint32_t PCM_FORMAT_24;
extern const uint32_t PCM_FORMAT_28;
extern const uint32_t PCM_FORMAT_32;

extern const int32_t SAMPLE_RATE_48;

/*
 * Sample Buffer Controls...
 */
#define RECORD_DEVICE_KICKSTART_BUF_COUNT 2
#define PLAY_KICKSTART_BUFFER_COUNT 3
#define DEVICE_SHADOW_BUFFER_QUEUE_LEN 4
#define BUF_COUNT 16

#ifndef BUFFER_QUEUE_CACHE_ALIGN
#define BUFFER_QUEUE_CACHE_ALIGN 64
#endif

struct SampleFormat {
    uint32_t sampleRate;
    uint32_t framesPerBuf;
    uint16_t channels;
    uint16_t pcmFormat;       // 8 bit, 16 bit, 24 bit ...
    uint32_t representation;  // android extensions
};

struct AudioConfig {
    bool supportRecording;
    uint32_t sampleRate;
    uint32_t sampleBufferSize;
    int64_t delayInMillis;
    float decay;
};

struct SampleBuffer {
    uint8_t* buffer;   // audio sample container
    uint32_t capacity;   // buffer capacity in byte
    uint32_t size;  // audio sample size (n buf) in byte
};

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
    alignas(BUFFER_QUEUE_CACHE_ALIGN) atomic<int> readAtomic{0};
    alignas(BUFFER_QUEUE_CACHE_ALIGN) atomic<int> writeAtomic{0};
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

class AudioPlayer {

public:
    static void init(const SampleFormat& sampleFormat);
    static void free();

    static void start();
    static void stop();

    uint32_t getDeviceBufferCount();

private:
    SampleFormat sampleFormat;
    AudioBufferQueue* freeBuffer = nullptr;
    AudioBufferQueue* playBuffer = nullptr;
    AudioBufferQueue* devShadowBuffer = nullptr;

    void* context = nullptr;

    SampleBuffer silentBuffer;
    Mutex stopMutex;
};

class AudioRecorder {

public:
    explicit AudioRecorder(SampleFormat* sampleFormat, SLEngineItf engineItf);
    ~AudioRecorder();

    SLboolean start();

    SLboolean stop();

    void setBufferQueue(AudioBufferQueue* freeBuffer, AudioBufferQueue* recBuffer);

    void processSLCallback(SLAndroidSimpleBufferQueueItf bufferQueue);

    void registerCallback(AUDIO_ENGINE_CALLBACK callback, void* context);

    int32_t getDeviceBufferCount();

private:
    SLObjectItf recObjectItf;
    SLRecordItf recItf;
    SLAndroidSimpleBufferQueueItf recBufQueueItf;

    SampleFormat sampleFormat;
    AudioBufferQueue* freeBuffer;
    AudioBufferQueue* recBuffer;
    AudioBufferQueue* devShadowBuffer;
    uint32_t audioBufCount;

    AUDIO_ENGINE_CALLBACK callback;

    void* context;

};

class Audio {

public:
    static void init();
    static void free();

    static void startPlayer();
    static void stopPlayer();

    static void startRecorder();
    static void stopRecorder();

private:
    static void initPlayer();
    static void initRecorder();

};

#endif //CONNECT_AUDIO_HPP
