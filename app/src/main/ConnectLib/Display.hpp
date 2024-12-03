//
// Created by cheerwizard on 28.11.24.
//

#ifndef CONNECT_DISPLAY_HPP
#define CONNECT_DISPLAY_HPP

class Display {

public:
    inline static int32_t width, height, format;

    static void init();
    static void terminate();

    static void swapChain();

};

#endif //CONNECT_DISPLAY_HPP
