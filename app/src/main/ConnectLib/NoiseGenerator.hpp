//
// Created by mecha on 02.12.2024.
//

#ifndef CROSSSOUND_NOISEGENERATOR_HPP
#define CROSSSOUND_NOISEGENERATOR_HPP

#include <Types.hpp>

#include "oboe/oboe.h"

class NoiseGenerator : public oboe::AudioStreamDataCallback, oboe::AudioStreamErrorCallback {

public:
    oboe::Result open();

    oboe::Result close();

    oboe::Result start();

    oboe::Result stop();

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream* audioStream, void* audioData, int32_t numFrames) override;

    void onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error) override;

private:
    oboe::AudioStream stream;
};

#endif //CROSSSOUND_NOISEGENERATOR_HPP