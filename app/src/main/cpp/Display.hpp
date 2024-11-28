//
// Created by cheerwizard on 28.11.24.
//

#ifndef CONNECT_DISPLAY_HPP
#define CONNECT_DISPLAY_HPP

#include "Graphics.hpp"

class Application;

class Display {

public:
    int32_t width, height, format;
    Graphics* graphics = nullptr;
    EGLDisplay display;
    EGLSurface surface;
    EGLConfig config;

    Display(Application* application, int32_t width, int32_t height);
    ~Display();

    void swap();
    void render();

private:
    void terminate();

};

#endif //CONNECT_DISPLAY_HPP
