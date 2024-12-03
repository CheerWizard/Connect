//
// Created by cheerwizard on 02.12.24.
//

#include "AudioPlayer.hpp"

/*
 * Called by OpenSL SimpleBufferQueue for every audio buffer played
 * directly pass thru to our handler.
 * The regularity of this callback from openSL/Android System affects
 * playback continuity. If it does not callback in the regular time
 * slot, you are under big pressure for audio processing[here we do
 * not do any filtering/mixing]. Callback from fast audio path are
 * much more regular than other audio paths by my observation. If it
 * very regular, you could buffer much less audio samples between
 * recorder and player, hence lower latency.
 */
void bufferQueuePlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void* context) {
    static_cast<AudioPlayer*>(context)->processSLCallback(bufferQueue);
}

void AudioPlayer::processSLCallback(SLAndroidSimpleBufferQueueItf bufferQueue) {
    lock_guard<mutex> lock(stopMutex);

    // retrieve the finished device buffers and put onto the free queue
    // so recorder could re-use it
    SampleBuffer* sampleBuffer;
    if (!devShadowBuffer->front(&sampleBuffer)) {
        /*
         * This should not happen: we got a callback,
         * but we have no buffer in deviceShadowedQueue
         * we lost buffers this way...(ERROR)
         */
        if (callback) {
            uint32_t count;
            callback(context, AUDIO_ENGINE_SERVICE_MSG_RETRIEVE_DUMP_BUFS, &count);
        }

        return;
    }
    devShadowBuffer->pop();

    if (sampleBuffer != &silentBuffer) {
        sampleBuffer->size = 0;
        freeBuffer->push(sampleBuffer);

        if (!playBuffer->front(&sampleBuffer)) {
            LOG_WARN("Running out of the Audio buffers!");
            return;
        }

        devShadowBuffer->push(sampleBuffer);
        (*bufferQueue)->Enqueue(bufferQueue, sampleBuffer->buffer, sampleBuffer->size);
        playBuffer->pop();

        return;
    }

    if (playBuffer->getSize() < PLAY_KICKSTART_BUFFER_COUNT) {
        (*bufferQueue)->Enqueue(bufferQueue, sampleBuffer->buffer, sampleBuffer->size);
        devShadowBuffer->push(&silentBuffer);
        return;
    }

    ASSERT(PLAY_KICKSTART_BUFFER_COUNT <= (DEVICE_SHADOW_BUFFER_QUEUE_LEN - devShadowBuffer->getSize()),
           "Dev shadow buffer is smaller then play kickstart buffer");

    for (int32_t idx = 0; idx < PLAY_KICKSTART_BUFFER_COUNT; idx++) {
        playBuffer->front(&sampleBuffer);
        playBuffer->pop();
        devShadowBuffer->push(sampleBuffer);
        (*bufferQueue)->Enqueue(bufferQueue, sampleBuffer->buffer, sampleBuffer->size);
    }
}

AudioPlayer::AudioPlayer(SampleFormat* sampleFormat, SLEngineItf slEngine) {
    ASSERT(sampleFormat != nullptr, "Sample format is NULL!");

    SLresult result;

    this->sampleFormat = *sampleFormat;

    result = (*slEngine)->CreateOutputMix(slEngine, &outputMixObjectItf, 0, NULL, NULL);
    SLASSERT(result);

    // realize the output mix
    result = (*outputMixObjectItf)->Realize(outputMixObjectItf, SL_BOOLEAN_FALSE);
    SLASSERT(result);

    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue locatorBufferQueue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
            DEVICE_SHADOW_BUFFER_QUEUE_LEN
    };

    SLAndroidDataFormat_PCM_EX pcmFormat;
    setSampleFormat(this->sampleFormat, &pcmFormat);
    SLDataSource audioSrc = { &locatorBufferQueue, &pcmFormat };

    // configure audio sink
    SLDataLocator_OutputMix locatorOutMix = {
            SL_DATALOCATOR_OUTPUTMIX,
            outputMixObjectItf
    };
    SLDataSink audioSnk = { &locatorOutMix, NULL };
    /*
     * create fast path audio player: SL_IID_BUFFERQUEUE and SL_IID_VOLUME
     * and other non-signal processing interfaces are ok.
     */
    SLInterfaceID ids[2] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME};
    SLboolean req[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*slEngine)->CreateAudioPlayer(
            slEngine, &playerObjectItf, &audioSrc, &audioSnk,
            sizeof(ids) / sizeof(ids[0]), ids, req
    );
    SLASSERT(result);

    // realize the player
    result = (*playerObjectItf)->Realize(playerObjectItf, SL_BOOLEAN_FALSE);
    SLASSERT(result);

    // get the play interface
    result = (*playerObjectItf)
            ->GetInterface(playerObjectItf, SL_IID_PLAY, &playItf);
    SLASSERT(result);

    // get the buffer queue interface
    result = (*playerObjectItf)
            ->GetInterface(playerObjectItf, SL_IID_BUFFERQUEUE,
                           &playBufferQueueItf);
    SLASSERT(result);

    // register callback on the buffer queue
    result = (*playBufferQueueItf)
            ->RegisterCallback(playBufferQueueItf, bufferQueuePlayerCallback, this);
    SLASSERT(result);

    result = (*playItf)->SetPlayState(playItf, SL_PLAYSTATE_STOPPED);
    SLASSERT(result);

    // create an empty queue to track deviceQueue
    devShadowBuffer = new AudioBufferQueue(DEVICE_SHADOW_BUFFER_QUEUE_LEN);
    ASSERT(devShadowBuffer != nullptr, "Device shadow buffer is NULL!");

    silentBuffer.capacity = (pcmFormat.containerSize >> 3) * pcmFormat.numChannels *
                      sampleFormat->framesPerBuf;
    silentBuffer.buffer = new uint8_t[silentBuffer.capacity];
    memset(silentBuffer.buffer, 0, silentBuffer.capacity);
    silentBuffer.size = silentBuffer.capacity;
}

AudioPlayer::~AudioPlayer() {
    lock_guard<mutex> lock(stopMutex);

    // destroy buffer queue audio player object, and invalidate all associated
    // interfaces
    if (playerObjectItf != nullptr) {
        (*playerObjectItf)->Destroy(playerObjectItf);
    }

    // Consume all non-completed audio buffers
    SampleBuffer* sampleBuffer = NULL;
    while (devShadowBuffer->front(&sampleBuffer)) {
        sampleBuffer->size = 0;
        devShadowBuffer->pop();
        if (sampleBuffer != &silentBuffer) {
            freeBuffer->push(sampleBuffer);
        }
    }
    delete devShadowBuffer;

    while (playBuffer->front(&sampleBuffer)) {
        sampleBuffer->size = 0;
        playBuffer->pop();
        freeBuffer->push(sampleBuffer);
    }

    // destroy output mix object, and invalidate all associated interfaces
    if (outputMixObjectItf) {
        (*outputMixObjectItf)->Destroy(outputMixObjectItf);
    }

    delete[] silentBuffer.buffer;
}

void AudioPlayer::setBufferQueues(AudioBufferQueue* playBuffer, AudioBufferQueue* freeBuffer) {
    this->playBuffer = playBuffer;
    this->freeBuffer = freeBuffer;
}

SLresult AudioPlayer::start() {
    SLuint32 state;
    SLresult result = (*playItf)->GetPlayState(playItf, &state);

    if (result != SL_RESULT_SUCCESS) {
        return SL_BOOLEAN_FALSE;
    }

    if (state == SL_PLAYSTATE_PLAYING) {
        return SL_BOOLEAN_TRUE;
    }

    result = (*playItf)->SetPlayState(playItf, SL_PLAYSTATE_STOPPED);
    SLASSERT(result);

    result =(*playBufferQueueItf)
            ->Enqueue(playBufferQueueItf, silentBuffer.buffer, silentBuffer.size);
    SLASSERT(result);

    devShadowBuffer->push(&silentBuffer);

    result = (*playItf)->SetPlayState(playItf, SL_PLAYSTATE_PLAYING);
    SLASSERT(result);

    return SL_BOOLEAN_TRUE;
}

void AudioPlayer::stop(void) {
    SLuint32 state;

    SLresult result = (*playItf)->GetPlayState(playItf, &state);
    SLASSERT(result);

    if (state == SL_PLAYSTATE_STOPPED) return;

    lock_guard<mutex> lock(stopMutex);

    result = (*playItf)->SetPlayState(playItf, SL_PLAYSTATE_STOPPED);
    SLASSERT(result);

    (*playBufferQueueItf)->Clear(playBufferQueueItf);
}

void AudioPlayer::registerCallback(AUDIO_ENGINE_CALLBACK callback, void* context) {
    this->callback = callback;
    this->context = context;
}

uint32_t AudioPlayer::getDeviceBufferCount() {
    return devShadowBuffer->getSize();
}