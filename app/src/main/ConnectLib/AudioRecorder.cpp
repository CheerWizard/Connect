//
// Created by cheerwizard on 02.12.24.
//

#include "AudioRecorder.hpp"

/*
 * bufferQueueRecorderCallback(): called for every buffer is full;
 *                       pass directly to handler
 */
void bufferQueueRecorderCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void* rec) {
    static_cast<AudioRecorder*>(rec)->processSLCallback(bufferQueue);
}

void AudioRecorder::processSLCallback(SLAndroidSimpleBufferQueueItf bufferQueue) {
    ASSERT(bufferQueue == recBufQueueItf, "Recording buffer queue is not corresponding to callback");
    SampleBuffer* sampleBuffer = nullptr;

    devShadowBuffer->front(&sampleBuffer);
    devShadowBuffer->pop();
    sampleBuffer->size = sampleBuffer->capacity;

    callback(context, AUDIO_ENGINE_SERVICE_MSG_RECORDED_AUDIO_AVAILABLE, sampleBuffer);
    recBuffer->push(sampleBuffer);

    SampleBuffer* freeSampleBuffer = nullptr;
    while (freeBuffer->front(&freeSampleBuffer) && devShadowBuffer->push(freeSampleBuffer)) {
        freeBuffer->pop();
        SLresult result = (*bufferQueue)->Enqueue(bufferQueue, freeSampleBuffer->buffer, freeSampleBuffer->capacity);
        SLASSERT(result);
    }

    ++audioBufCount;

    // should leave the device to sleep to save power if no buffers
    if (devShadowBuffer->getSize() == 0) {
        (*recItf)->SetRecordState(recItf, SL_RECORDSTATE_STOPPED);
    }
}

AudioRecorder::AudioRecorder(SampleFormat *sampleFormat, SLEngineItf slEngine)
{
    SLresult result;

    this->sampleFormat = *sampleFormat;
    SLAndroidDataFormat_PCM_EX pcmFormat;
    setSampleFormat(this->sampleFormat, &pcmFormat);

    // configure audio source
    SLDataLocator_IODevice locateDevice = {
            SL_DATALOCATOR_IODEVICE,
            SL_IODEVICE_AUDIOINPUT,
            SL_DEFAULTDEVICEID_AUDIOINPUT,
            NULL
    };

    SLDataSource audioSrc = {&locateDevice, NULL};

    // configure audio sink
    SLDataLocator_AndroidSimpleBufferQueue locateBufferQueue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
            DEVICE_SHADOW_BUFFER_QUEUE_LEN
    };

    SLDataSink audioSnk = {&locateBufferQueue, &pcmFormat};

    // init audio recorder
    // (requires the RECORD_AUDIO permission)
    const SLInterfaceID id[2] = {
            SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
            SL_IID_ANDROIDCONFIGURATION
    };
    const SLboolean req[2] = {
            SL_BOOLEAN_TRUE,
            SL_BOOLEAN_TRUE
    };
    result = (*slEngine)->CreateAudioRecorder(
            slEngine, &recObjectItf, &audioSrc, &audioSnk,
            sizeof(id) / sizeof(id[0]), id, req
    );
    SLASSERT(result);

    // Configure the voice recognition preset which has no
    // signal processing for lower latency.
    SLAndroidConfigurationItf inputConfig;
    result = (*recObjectItf)->GetInterface(recObjectItf, SL_IID_ANDROIDCONFIGURATION, &inputConfig);

    if (SL_RESULT_SUCCESS == result) {
        SLuint32 presetValue = SL_ANDROID_RECORDING_PRESET_VOICE_RECOGNITION;
        (*inputConfig)->SetConfiguration(inputConfig, SL_ANDROID_KEY_RECORDING_PRESET, &presetValue, sizeof(SLuint32));
    }

    result = (*recObjectItf)->Realize(recObjectItf, SL_BOOLEAN_FALSE);
    SLASSERT(result);

    result = (*recObjectItf)->GetInterface(recObjectItf, SL_IID_RECORD, &recItf);
    SLASSERT(result);

    result = (*recObjectItf)->GetInterface(recObjectItf, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &recBufQueueItf);
    SLASSERT(result);

    result = (*recBufQueueItf)->RegisterCallback(recBufQueueItf, bufferQueueRecorderCallback, this);
    SLASSERT(result);

    devShadowBuffer = new AudioBufferQueue(DEVICE_SHADOW_BUFFER_QUEUE_LEN);
    ASSERT(devShadowBuffer != nullptr, "Device shadow buffer is NULL!");
}

SLboolean AudioRecorder::start() {
    ASSERT(freeBuffer != nullptr, "Free buffer is NULL!");
    ASSERT(recBuffer != nullptr, "Recording buffer is NULL!");
    ASSERT(devShadowBuffer != nullptr, "Device shadow buffer is NULL!");

    audioBufCount = 0;

    SLresult result;

    // in case already recording, stop recording and clear buffer queue
    result = (*recItf)->SetRecordState(recItf, SL_RECORDSTATE_STOPPED);
    SLASSERT(result);

    result = (*recBufQueueItf)->Clear(recBufQueueItf);
    SLASSERT(result);

    for (int i = 0; i < RECORD_DEVICE_KICKSTART_BUF_COUNT; i++) {
        SampleBuffer* sampleBuffer = NULL;

        if (!freeBuffer->front(&sampleBuffer)) {
            LOG_ERR("OutOfFreeBuffers @ startingRecording @ (%d)", i);
            break;
        }

        freeBuffer->pop();
        ASSERT(sampleBuffer->buffer && sampleBuffer->capacity && !sampleBuffer->size, "SampleBuffer is empty!");

        result = (*recBufQueueItf)->Enqueue(recBufQueueItf, sampleBuffer->buffer, sampleBuffer->capacity);
        SLASSERT(result);

        devShadowBuffer->push(sampleBuffer);
    }

    result = (*recItf)->SetRecordState(recItf, SL_RECORDSTATE_RECORDING);
    SLASSERT(result);

    return (result == SL_RESULT_SUCCESS ? SL_BOOLEAN_TRUE : SL_BOOLEAN_FALSE);
}

SLboolean AudioRecorder::stop() {
    // in case already recording, stop recording and clear buffer queue
    SLuint32 curState;

    SLresult result = (*recItf)->GetRecordState(recItf, &curState);
    SLASSERT(result);

    if (curState == SL_RECORDSTATE_STOPPED) {
        return SL_BOOLEAN_TRUE;
    }

    result = (*recItf)->SetRecordState(recItf, SL_RECORDSTATE_STOPPED);
    SLASSERT(result);

    result = (*recBufQueueItf)->Clear(recBufQueueItf);
    SLASSERT(result);

    return SL_BOOLEAN_TRUE;
}

AudioRecorder::~AudioRecorder() {
    // destroy audio recorder object, and invalidate all associated interfaces
    if (recObjectItf != NULL) {
        (*recObjectItf)->Destroy(recObjectItf);
    }

    if (devShadowBuffer) {
        SampleBuffer* sampleBuffer = NULL;
        while (devShadowBuffer->front(&sampleBuffer)) {
            devShadowBuffer->pop();
            freeBuffer->push(sampleBuffer);
        }
        delete (devShadowBuffer);
    }
}

void AudioRecorder::setBufferQueue(AudioBufferQueue* freeBuffer, AudioBufferQueue* recBuffer) {
    ASSERT(freeBuffer != nullptr, "Free buffer is NULL!");
    ASSERT(recBuffer != nullptr, "Recording buffer is NULL!");
    this->freeBuffer = freeBuffer;
    this->recBuffer = recBuffer;
}

void AudioRecorder::registerCallback(AUDIO_ENGINE_CALLBACK callback, void* context) {
    this->callback= callback;
    this->context = context;
}

int32_t AudioRecorder::getDeviceBufferCount() {
    return devShadowBuffer->getSize();
}