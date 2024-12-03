//
// Created by cheerwizard on 02.12.24.
//

#include "Audio.hpp"
#include "AudioPlayer.hpp"
#include "AudioRecorder.hpp"
#include "AudioEffect.hpp"
#include <jni.h>

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