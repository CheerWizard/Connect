//
// Created by mecha on 02.12.2024.
//

#include "NoiseGenerator.hpp"
#include "Logger.hpp"

oboe::Result NoiseGenerator::open() {
    oboe::AudioStreamBuilder builder;
    oboe::Result result = builder.setSharingMode(oboe::SharingMode::Exclusive)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setFormat(oboe::AudioFormat::Float)
            ->setChannelCount(2)
            ->setDataCallback(this)
            ->setErrorCallback(this)
            ->openStream(stream);
    return result;
}

oboe::Result NoiseGenerator::start() {
    return stream->requestStart();
}

oboe::Result NoiseGenerator::stop() {
    return stream->requestStop();
}

oboe::Result NoiseGenerator::close() {
    return stream->close();
}

DataCallbackResult NoiseGenerator::onAudioReady(AudioStream* audioStream, void* audioData,int32_t numFrames) {
    float* output = (float*) audioData;
    // Fill buffer with random numbers to create "white noise".
    int numSamples = numFrames * 2;
    for (int i = 0; i < numSamples; i++) {
        *output++ = (float) ((drand48() - 0.5) * 0.6);
    }
    return oboe::DataCallbackResult::Continue;
}

void NoiseGenerator::onErrorAfterClose(oboe::AudioStream* oboeStream, oboe::Result error) {
    LOG_ERR(oboe::convertToText(error));
    if (open() == Result::OK) {
        start();
    }
}