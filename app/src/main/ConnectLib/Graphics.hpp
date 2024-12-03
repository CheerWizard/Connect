//
// Created by cheerwizard on 02.12.24.
//

#ifndef CONNECT_GRAPHICS_HPP
#define CONNECT_GRAPHICS_HPP

#include "GraphicsCore.hpp"

class Graphics {

public:
    inline static Vec4<float> clearColor = {0.5, 0.5, 0.5, 1};
    inline static vector<Shader*> shaders;

    static void init();
    static void free();
    static void render();

};

#endif //CONNECT_GRAPHICS_HPP
