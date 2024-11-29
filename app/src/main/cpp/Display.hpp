//
// Created by cheerwizard on 28.11.24.
//

#ifndef CONNECT_DISPLAY_HPP
#define CONNECT_DISPLAY_HPP

#include "Graphics.hpp"
#include "Types.hpp"

class Application;

class Display {

public:
    int32_t width, height, format;
    Graphics* graphics = nullptr;

    Display(Application* application);
    ~Display();

    void swap();
    void render();

private:
    void terminate();

};

#endif //CONNECT_DISPLAY_HPP
