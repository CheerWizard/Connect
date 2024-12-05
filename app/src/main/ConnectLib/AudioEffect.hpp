//
// Created by cheerwizard on 02.12.24.
//

#ifndef CONNECT_AUDIOEFFECT_HPP
#define CONNECT_AUDIOEFFECT_HPP

#include "Audio.hpp"

/**
 * An audio delay effect:
 *   - decay is for feedback(echo)weight
 *   - delay time is adjustable
 */
class AudioDelay {

public:
    explicit AudioDelay(int32_t sampleRate, int32_t channelCount, uint32_t format, size_t delayTimeInMs, float weight);

    ~AudioDelay();

    bool setDelayTime(size_t delayTimeInMiliSec);

    size_t getDelayTime() const;

    void setDecayWeight(float weight);

    float getDecayWeight() const;

    void process(int16_t* liveAudio, int32_t numFrames);

private:
    void allocateBuffer();

    int32_t sampleRate = SAMPLE_RATE_48;
    int32_t channelCount = 2;
    uint32_t format = PCM_FORMAT_16;
    size_t delayTime = 0;
    float decayWeight = 0.5;
    void* buffer = nullptr;
    size_t bufCapacity = 0;
    size_t bufSize = 0;
    size_t curPos = 0;
    Mutex mutex;
    int32_t feedbackFactor;
    int32_t liveAudioFactor;
};

#endif //CONNECT_AUDIOEFFECT_HPP
