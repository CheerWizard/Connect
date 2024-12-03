//
// Created by cheerwizard on 02.12.24.
//

#ifndef CONNECT_AUDIO_HPP
#define CONNECT_AUDIO_HPP

#include "AudioCore.hpp"

class Audio {

public:
    static void init();
    static void free();

    static void startPlayer();
    static void stopPlayer();

    static void startRecorder();
    static void stopRecorder();

private:
    static void initPlayer();
    static void initRecorder();

};

#endif //CONNECT_AUDIO_HPP
