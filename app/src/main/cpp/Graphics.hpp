//
// Created by cheerwizard on 28.11.24.
//

#ifndef CONNECT_GRAPHICS_HPP
#define CONNECT_GRAPHICS_HPP

#include "RenderContext.hpp"

class Display;

struct Color {
    float r, g, b, a;
};

class Graphics {

public:
    Color clearColor = { 1, 1, 1, 1 };

    Graphics(Application* application, Display* display);
    ~Graphics();

    void render();

private:
    Display* display;
    RenderContext* context;
};


#endif //CONNECT_GRAPHICS_HPP
