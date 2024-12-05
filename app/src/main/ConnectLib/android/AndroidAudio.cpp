//
// Created by cheerwizard on 05.12.24.
//

#include "Audio.hpp"
#include "AudioPlayer.hpp"
#include "AudioRecorder.hpp"
#include "AudioEffect.hpp"

#include <jni.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

const uint32_t PCM_FORMAT_8 = SL_PCMSAMPLEFORMAT_FIXED_8;
const uint32_t PCM_FORMAT_16 = SL_PCMSAMPLEFORMAT_FIXED_16;
const uint32_t PCM_FORMAT_20 = SL_PCMSAMPLEFORMAT_FIXED_20;
const uint32_t PCM_FORMAT_24 = SL_PCMSAMPLEFORMAT_FIXED_24;
const uint32_t PCM_FORMAT_28 = SL_PCMSAMPLEFORMAT_FIXED_28;
const uint32_t PCM_FORMAT_32 = SL_PCMSAMPLEFORMAT_FIXED_32;

const int32_t SAMPLE_RATE_48 = SL_SAMPLINGRATE_48;

void setSampleFormat(const SampleFormat &format, void *outFormat) {
    ASSERT(outFormat != nullptr, "SLES sample format is NULL!");

    SLAndroidDataFormat_PCM_EX* slesFormat = reinterpret_cast<SLAndroidDataFormat_PCM_EX*>(outFormat);

    memset(slesFormat, 0, sizeof(*slesFormat));

    slesFormat->formatType = SL_DATAFORMAT_PCM;
    slesFormat->sampleRate = format.sampleRate;
    slesFormat->endianness = SL_BYTEORDER_LITTLEENDIAN;
    slesFormat->bitsPerSample = format.pcmFormat;
    slesFormat->containerSize = format.pcmFormat;

    // Only support 2 channels
    // For channelMask, refer to wilhelm/src/android/channels.c for details
    if (format.channels <= 1) {
        slesFormat->numChannels = 1;
        slesFormat->channelMask = SL_SPEAKER_FRONT_LEFT;
    } else {
        slesFormat->numChannels = 2;
        slesFormat->channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    }

    slesFormat->representation = format.representation;

    switch (slesFormat->representation) {
        case SL_ANDROID_PCM_REPRESENTATION_UNSIGNED_INT:
            slesFormat->bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_8;
            slesFormat->containerSize = SL_PCMSAMPLEFORMAT_FIXED_8;
            slesFormat->formatType = SL_ANDROID_DATAFORMAT_PCM_EX;
            break;
        case SL_ANDROID_PCM_REPRESENTATION_SIGNED_INT:
            slesFormat->bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;  // supports 16, 24, and 32
            slesFormat->containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
            slesFormat->formatType = SL_ANDROID_DATAFORMAT_PCM_EX;
            break;
        case SL_ANDROID_PCM_REPRESENTATION_FLOAT:
            slesFormat->bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_32;
            slesFormat->containerSize = SL_PCMSAMPLEFORMAT_FIXED_32;
            slesFormat->formatType = SL_ANDROID_DATAFORMAT_PCM_EX;
            break;
        case 0:
            break;
    }
}

bool onAudioEngineMsgReceived(void* context, uint32_t msg, void* data);

struct EchoAudioEngine {
    SLmilliHertz fastPathSampleRate;
    uint32_t fastPathFramesPerBuf;
    uint16_t sampleChannels;
    uint16_t bitsPerSample;

    SLObjectItf slEngineObj;
    SLEngineItf slEngineItf;

    AudioRecorder* recorder;
    AudioPlayer* player;
    AudioBufferQueue* freeBufQueue;
    AudioBufferQueue* recBufQueue;

    SampleBuffer* buffers;
    uint32_t bufferCount;
    uint32_t frameCount;

    int64_t echoDelay;
    float echoDecay;

    AudioDelay* delayEffect;

    AudioConfig config;
};

static EchoAudioEngine engine;

//// pre-recorded sound clips, both are 8 kHz mono 16-bit signed little endian
//static const char hello[] =
//#include "clips/ClipHello.hpp"
//;
//static const char android[] =
//#include "clips/ClipAndroid.hpp"
//;

//#define AUDIO_URI_FREESOUND_PREVIEW "http://www.freesound.org/data/previews/18/18765_18799-lq.mp3"

void setAudioConfig() {
    JNIEnv* jni;
    ANativeActivity* activity = app->activity;

    activity->vm->AttachCurrentThread(&jni, nullptr);

    jclass activityClass = jni->GetObjectClass(activity->clazz);
    jmethodID getAudioConfigId = jni->GetMethodID(activityClass, "getAudioConfig", "()Lcom/cws/connect/AudioConfig;");

    jobject audioConfigObject = jni->CallObjectMethod(activity->clazz, getAudioConfigId);
    jclass audioConfigClass = jni->GetObjectClass(audioConfigObject);

    engine.config.supportRecording = jni->GetBooleanField(audioConfigObject, jni->GetFieldID(audioConfigClass, "supportRecording", "Z"));
    engine.config.sampleRate = jni->GetIntField(audioConfigObject, jni->GetFieldID(audioConfigClass, "sampleRate", "I"));
    engine.config.sampleBufferSize = jni->GetIntField(audioConfigObject, jni->GetFieldID(audioConfigClass, "sampleBufferSize", "I"));
    engine.config.delayInMillis = jni->GetLongField(audioConfigObject, jni->GetFieldID(audioConfigClass, "delayInMillis", "J"));
    engine.config.decay = jni->GetFloatField(audioConfigObject, jni->GetFieldID(audioConfigClass, "decay", "F"));
}

void Audio::init() {
    SLresult result;

    setAudioConfig();

    engine.fastPathSampleRate = static_cast<SLmilliHertz>(engine.config.sampleRate) * 1000;
    engine.fastPathFramesPerBuf = static_cast<uint32_t>(engine.config.sampleBufferSize);
    engine.sampleChannels = AUDIO_SAMPLE_CHANNELS;
    engine.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;

    result = slCreateEngine(&engine.slEngineObj, 0, nullptr, 0, nullptr, nullptr);
    SLASSERT(result);

    result = (*engine.slEngineObj)->Realize(engine.slEngineObj, SL_BOOLEAN_FALSE);
    SLASSERT(result);

    result = (*engine.slEngineObj)->GetInterface(engine.slEngineObj, SL_IID_ENGINE, &engine.slEngineItf);
    SLASSERT(result);

    // compute the RECOMMENDED fast audio buffer size:
    //   the lower latency required
    //     *) the smaller the buffer should be (adjust it here) AND
    //     *) the less buffering should be before starting player AFTER
    //        receiving the recorder buffer
    //   Adjust the bufSize here to fit your bill [before it busts]
    uint32_t bufSize = engine.fastPathFramesPerBuf * engine.sampleChannels * engine.bitsPerSample;
    bufSize = (bufSize + 7) >> 3;  // bits --> byte
    engine.bufferCount = BUF_COUNT;
    engine.buffers = allocateSampleBuffer(engine.bufferCount, bufSize);
    ASSERT(engine.buffers, "SampleBuffer is NULL and not allocated!");

    engine.freeBufQueue = new AudioBufferQueue(engine.bufferCount);
    engine.recBufQueue = new AudioBufferQueue (engine.bufferCount);
    ASSERT(engine.freeBufQueue && engine.recBufQueue, "FreeBufferQueue or RecordBufferQueue is NULL!");
    for (uint32_t i = 0; i < engine.bufferCount; i++) {
        engine.freeBufQueue->push(&engine.buffers[i]);
    }

    engine.echoDelay = engine.config.delayInMillis;
    engine.echoDecay = engine.config.decay;
    engine.delayEffect = new AudioDelay(
            engine.fastPathSampleRate,
            engine.sampleChannels,
            engine.bitsPerSample,
            engine.echoDelay,
            engine.echoDecay
    );
    ASSERT(engine.delayEffect, "DelayEffect is NULL!");

    initPlayer();
    initRecorder();
}

void Audio::free() {
    delete engine.player;
    delete engine.recorder;
    delete engine.recBufQueue;
    delete engine.freeBufQueue;
    releaseSampleBufs(engine.buffers, engine.bufferCount);

    if (engine.slEngineObj) {
        (*engine.slEngineObj)->Destroy(engine.slEngineObj);
        engine.slEngineObj = nullptr;
        engine.slEngineItf = nullptr;
    }

    delete engine.delayEffect;
}

void Audio::initPlayer() {
    SampleFormat sampleFormat = {};
    memset(&sampleFormat, 0, sizeof(sampleFormat));
    sampleFormat.pcmFormat = (uint16_t) engine.bitsPerSample;
    sampleFormat.framesPerBuf = engine.fastPathFramesPerBuf;

    // SampleFormat.representation_ = SL_ANDROID_PCM_REPRESENTATION_SIGNED_INT;
    sampleFormat.channels = (uint16_t) engine.sampleChannels;
    sampleFormat.sampleRate = engine.fastPathSampleRate;

    engine.player = new AudioPlayer(&sampleFormat, engine.slEngineItf);
    ASSERT(engine.player, "AudioPlayer is NULL!");
    if (engine.player == nullptr) return;

    engine.player->setBufferQueues(engine.recBufQueue, engine.freeBufQueue);
    engine.player->registerCallback(onAudioEngineMsgReceived, (void *) &engine);
}

void Audio::initRecorder() {
    SampleFormat sampleFormat = {};
    memset(&sampleFormat, 0, sizeof(sampleFormat));
    sampleFormat.pcmFormat = static_cast<uint16_t>(engine.bitsPerSample);

// SampleFormat.representation_ = SL_ANDROID_PCM_REPRESENTATION_SIGNED_INT;
    sampleFormat.channels = engine.sampleChannels;
    sampleFormat.sampleRate = engine.fastPathSampleRate;
    sampleFormat.framesPerBuf = engine.fastPathFramesPerBuf;
    engine.recorder = new AudioRecorder(&sampleFormat, engine.slEngineItf);
    ASSERT(engine.recorder, "AudioRecorder is NULL!");
    engine.recorder->setBufferQueue(engine.freeBufQueue, engine.recBufQueue);
    engine.recorder->registerCallback(onAudioEngineMsgReceived, (void *) &engine);
}

void Audio::startPlayer() {
    if (engine.player->start() == SL_BOOLEAN_FALSE) {
        LOG_ERR("Failed to start AudioPlayer!");
        return;
    }
}

void Audio::startRecorder() {
    if (engine.recorder->start() == SL_BOOLEAN_FALSE) {
        LOG_ERR("Failed to start AudioRecorder!");
        return;
    }
}

void Audio::stopPlayer() {
    engine.player->stop();
}

void Audio::stopRecorder() {
    engine.recorder->stop();
}

uint32_t getEngineBufferCount() {
    uint32_t count = engine.player->getDeviceBufferCount();
    count += engine.recorder->getDeviceBufferCount();
    count += engine.freeBufQueue->getSize();
    count += engine.recBufQueue->getSize();

    LOG_ERR("Buf Disrtibutions: PlayerDev=%d, RecDev=%d, FreeQ=%d, RecQ=%d",
            engine.player->getDeviceBufferCount(),
            engine.recorder->getDeviceBufferCount(),
            engine.freeBufQueue->getSize(),
            engine.recBufQueue->getSize()
    );

    if (count != engine.bufferCount) {
        LOG_ERR("Audio engine lost buffers among the queue(supposed = %d, found = %d)", BUF_COUNT, count);
    }

    return count;
}

bool onAudioEngineMsgReceived(void* context, uint32_t msg, void* data) {
    ASSERT(context == &engine, "Audio Engine context is invalid!");

    switch (msg) {
        case AUDIO_ENGINE_SERVICE_MSG_RETRIEVE_DUMP_BUFS: {
            *(static_cast<uint32_t*>(data)) = getEngineBufferCount();
            break;
        }

        case AUDIO_ENGINE_SERVICE_MSG_RECORDED_AUDIO_AVAILABLE: {
            SampleBuffer* buffers = static_cast<SampleBuffer*>(data);

            ASSERT(engine.fastPathFramesPerBuf == buffers->size / engine.sampleChannels / (engine.bitsPerSample / 8),
                   "Incorrect number of fast path frames per buffer!"
            );

            // adding audio delay effect
            engine.delayEffect->process(
                    reinterpret_cast<int16_t*>(buffers->buffer),
                    engine.fastPathFramesPerBuf
            );
            break;
        }

        default: return false;
    }

    return true;
}

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
    LOCK(stopMutex);

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
     * init fast path audio player: SL_IID_BUFFERQUEUE and SL_IID_VOLUME
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

    // init an empty queue to track deviceQueue
    devShadowBuffer = new AudioBufferQueue(DEVICE_SHADOW_BUFFER_QUEUE_LEN);
    ASSERT(devShadowBuffer != nullptr, "Device shadow buffer is NULL!");

    silentBuffer.capacity = (pcmFormat.containerSize >> 3) * pcmFormat.numChannels *
                            sampleFormat->framesPerBuf;
    silentBuffer.buffer = new uint8_t[silentBuffer.capacity];
    memset(silentBuffer.buffer, 0, silentBuffer.capacity);
    silentBuffer.size = silentBuffer.capacity;
}

AudioPlayer::~AudioPlayer() {
    LOCK(stopMutex);

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

    LOCK(stopMutex);

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