//
// Created by cheerwizard on 02.12.24.
//

#ifndef CONNECT_AUDIOPLAYER_HPP
#define CONNECT_AUDIOPLAYER_HPP

#include "AudioBackend.hpp"
#include "AudioBufferQueue.hpp"

class AudioPlayer {

public:
    explicit AudioPlayer(SampleFormat* sampleFormat, SLEngineItf engine);

    ~AudioPlayer();

    void setBufferQueues(AudioBufferQueue* playBuffer, AudioBufferQueue* freeBuffer);

    SLresult start();

    void stop();

    void processSLCallback(SLAndroidSimpleBufferQueueItf bufferQueue);

    uint32_t getDeviceBufferCount();

    void registerCallback(AUDIO_ENGINE_CALLBACK callback, void* context);

private:
    SLObjectItf outputMixObjectItf;
    SLObjectItf playerObjectItf;
    SLPlayItf playItf;
    SLAndroidSimpleBufferQueueItf playBufferQueueItf;

    SampleFormat sampleFormat;
    AudioBufferQueue* freeBuffer = nullptr;
    AudioBufferQueue* playBuffer = nullptr;
    AudioBufferQueue* devShadowBuffer = nullptr;

    AUDIO_ENGINE_CALLBACK callback = nullptr;

    void* context = nullptr;

    SampleBuffer silentBuffer;

    mutex stopMutex;

};
#endif //CONNECT_AUDIOPLAYER_HPP