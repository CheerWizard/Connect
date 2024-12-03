//
// Created by cheerwizard on 02.12.24.
//

#include "AudioCore.hpp"

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