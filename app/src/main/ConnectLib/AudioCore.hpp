//
// Created by cheerwizard on 02.12.24.
//

#ifndef CONNECT_AUDIOCORE_HPP
#define CONNECT_AUDIOCORE_HPP

/*
 * Audio Sample Controls...
 */
#define AUDIO_SAMPLE_CHANNELS 1

/*
 * Sample Buffer Controls...
 */
#define RECORD_DEVICE_KICKSTART_BUF_COUNT 2
#define PLAY_KICKSTART_BUFFER_COUNT 3
#define DEVICE_SHADOW_BUFFER_QUEUE_LEN 4
#define BUF_COUNT 16

/*
 * Interface for player and recorder to communicate with audio engine
 */
#define AUDIO_ENGINE_SERVICE_MSG_KICKSTART_PLAYER 1
#define AUDIO_ENGINE_SERVICE_MSG_RETRIEVE_DUMP_BUFS 2
#define AUDIO_ENGINE_SERVICE_MSG_RECORDED_AUDIO_AVAILABLE 3
typedef bool (*AUDIO_ENGINE_CALLBACK)(void* context, uint32_t msg, void* data);

struct SampleFormat {
    uint32_t sampleRate;
    uint32_t framesPerBuf;
    uint16_t channels;
    uint16_t pcmFormat;       // 8 bit, 16 bit, 24 bit ...
    uint32_t representation;  // android extensions
};

extern void setSampleFormat(const SampleFormat& format, void* outFormat);

#if defined(ANDROID)

#define SLASSERT(x)                   \
  do {                                \
    assert(SL_RESULT_SUCCESS == (x)); \
    (void)(x);                        \
  } while (0)

#endif

struct AudioConfig {
    bool supportRecording;
    uint32_t sampleRate;
    uint32_t sampleBufferSize;
    int64_t delayInMillis;
    float decay;
};

#endif //CONNECT_AUDIOCORE_HPP