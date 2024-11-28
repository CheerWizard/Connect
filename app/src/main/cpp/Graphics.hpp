//
// Created by cheerwizard on 28.11.24.
//

#ifndef CONNECT_GRAPHICS_HPP
#define CONNECT_GRAPHICS_HPP

#include <EGL/egl.h>
#include <GLES/gl.h>

class Display;

struct Color {
    float r, g, b, a;
};

class Graphics {

public:
    Color clearColor = { 1, 1, 1, 1 };

    Graphics(const Display* const display);
    ~Graphics();

    void render();

private:
    EGLContext context;
    const Display* const display;

};


#endif //CONNECT_GRAPHICS_HPP
