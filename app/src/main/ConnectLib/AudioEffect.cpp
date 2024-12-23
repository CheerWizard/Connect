//
// Created by cheerwizard on 02.12.24.
//

#include "AudioEffect.hpp"

/*
 * Mixing Audio in integer domain to avoid FP calculation
 *   (FG * ( MixFactor * 16 ) + BG * ( (1.0f-MixFactor) * 16 )) / 16
 */
static const int32_t FLOAT_TO_INT_MAP_FACTOR = 128;
static const uint32_t MS_PER_SEC = 1000;

/**
 * Constructor for AudioDelay
 * @param sampleRate
 * @param channelCount
 * @param format
 * @param delayTimeInMs
 */
AudioDelay::AudioDelay(
    int32_t sampleRate,
    int32_t channelCount,
    uint32_t format,
    size_t delayTimeInMs,
    float decayWeight
) :
sampleRate(sampleRate),
channelCount(channelCount),
format(format),
delayTime(delayTimeInMs),
decayWeight(decayWeight)
{
    feedbackFactor = static_cast<int32_t>(decayWeight * FLOAT_TO_INT_MAP_FACTOR);
    liveAudioFactor = FLOAT_TO_INT_MAP_FACTOR - feedbackFactor;
    allocateBuffer();
}

/**
 * Destructor
 */
AudioDelay::~AudioDelay() {
    if (buffer) delete static_cast<uint8_t*>(buffer);
}

/**
 * Configure for delay time ( in miliseconds ), dynamically adjustable
 * @param delayTimeInMS in miliseconds
 * @return true if delay time is set successfully
 */
bool AudioDelay::setDelayTime(size_t delayTimeInMS) {
    if (delayTimeInMS == delayTime) return true;

    LOCK(mutex);

    if (buffer) {
        delete static_cast<uint8_t*>(buffer);
        buffer = nullptr;
    }

    delayTime = delayTimeInMS;

    allocateBuffer();

    return buffer != nullptr;
}

/**
 * Internal helper function to allocate buffer for the delay
 *  - calculate the buffer size for the delay time
 *  - allocate and zero out buffer (0 means silent audio)
 *  - configure bufSize_ to be size of audioFrames
 */
void AudioDelay::allocateBuffer() {
    float floatDelayTime = (float)delayTime / MS_PER_SEC;
    float fNumFrames = floatDelayTime * (float)sampleRate / MS_PER_SEC;
    size_t sampleCount = static_cast<uint32_t>(fNumFrames + 0.5f) * channelCount;

    uint32_t bytePerSample = format / 8;
    assert(bytePerSample <= 4 && bytePerSample);

    uint32_t bytePerFrame = channelCount * bytePerSample;

    // get bufCapacity in bytes
    bufCapacity = sampleCount * bytePerSample;
    bufCapacity =
            ((bufCapacity + bytePerFrame - 1) / bytePerFrame) * bytePerFrame;

    buffer = new uint8_t[bufCapacity];
    assert(buffer);

    memset(buffer, 0, bufCapacity);
    curPos = 0;

    // bufSize_ is in Frames ( not samples, not bytes )
    bufSize = bufCapacity / bytePerFrame;
}

size_t AudioDelay::getDelayTime() const { return delayTime; }

/**
 * setDecayWeight(): set the decay factor
 * ratio: value of 0.0 -- 1.0f;
 *
 * the calculation is in integer ( not in float )
 * for performance purpose
 */
void AudioDelay::setDecayWeight(float weight) {
    if (weight > 0.0f && weight < 1.0f) {
        float feedback = (weight * FLOAT_TO_INT_MAP_FACTOR + 0.5f);
        feedbackFactor = static_cast<int32_t>(feedback);
        liveAudioFactor = FLOAT_TO_INT_MAP_FACTOR - feedbackFactor;
    }
}

float AudioDelay::getDecayWeight() const { return decayWeight; }

/**
 * process() filter live audio with "echo" effect:
 *   delay time is run-time adjustable
 *   decay time could also be adjustable, but not used
 *   in this sample, hardcoded to .5
 *
 * @param liveAudio is recorded audio stream
 * @param channelCount for liveAudio, must be 2 for stereo
 * @param numFrames is length of liveAudio in Frames ( not in byte )
 */
void AudioDelay::process(int16_t* liveAudio, int32_t numFrames) {
    if (feedbackFactor == 0 || bufSize < numFrames) {
        return;
    }

    LOCK(mutex);

    if (numFrames + curPos > bufSize) {
        curPos = 0;
    }

    // process every sample
    int32_t sampleCount = channelCount * numFrames;
    int16_t* samples = &static_cast<int16_t*>(buffer)[curPos * channelCount];
    for (size_t idx = 0; idx < sampleCount; idx++) {
#if 1
        int32_t curSample =
                (samples[idx] * feedbackFactor + liveAudio[idx] * liveAudioFactor) /
                FLOAT_TO_INT_MAP_FACTOR;
        if (curSample > SHRT_MAX)
            curSample = SHRT_MAX;
        else if (curSample < SHRT_MIN)
            curSample = SHRT_MIN;

        liveAudio[idx] = samples[idx];
        samples[idx] = static_cast<int16_t>(curSample);
#else
        // Pure delay
    int16_t tmp = liveAudio[idx];
    liveAudio[idx] = samples[idx];
    samples[idx] = tmp;
#endif
    }

    curPos += numFrames;
}