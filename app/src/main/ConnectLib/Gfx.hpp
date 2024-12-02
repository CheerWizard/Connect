//
// Created by cheerwizard on 28.11.24.
//

#ifndef CONNECT_GFX_HPP
#define CONNECT_GFX_HPP

#include "Types.hpp"

struct Color {
    float r, g, b, a;
};

class Gfx {

public:
    inline static void* context = nullptr;
    inline static Color clearColor = { 1, 1, 1, 1 };

    static void init();
    static void free();

    static void clearBuffer(int bufferBits);

};


#endif //CONNECT_GFX_HPP
