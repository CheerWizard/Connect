//
// Created by cheerwizard on 02.12.24.
//

#ifndef CONNECT_AUDIORECORDER_HPP
#define CONNECT_AUDIORECORDER_HPP

#include "AudioBackend.hpp"
#include "AudioBufferQueue.hpp"

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

#endif //CONNECT_AUDIORECORDER_HPP