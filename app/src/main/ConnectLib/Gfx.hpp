//
// Created by cheerwizard on 28.11.24.
//

#ifndef CONNECT_GFX_HPP
#define CONNECT_GFX_HPP

#include "Types.hpp"
#include "Math.hpp"

struct Color {
    float r, g, b, a;
};

template<typename T>
struct Buffer {
    uint8_t type = 0;
    uint8_t slot = 0;
    vector<T> data;
};

struct Viewport {
    float left = 0;
    float top = 0;
    float width = 0;
    float height = 0;
    float minDepth = 0;
    float maxDepth = 1;
};

class GeometryBuffer {

public:
    uint32_t vertexArray = 0;
    uint32_t vertexBuffer = 0;
    uint32_t indexBuffer = 0;

    void init();
    void free();
    GeometryDrawData addGeometry(const GeometryData& geometryData);
    void removeGeometry(const GeometryDrawData& geometryDrawData);
    void flush();

};

struct Camera {};

struct Entity {};

class Render {

public:
    inline static GeometryBuffer geometryBuffer = {};

    static void init();
    static void free();
    static void renderEntities();

};

class Gfx {

public:
    inline static void* context = nullptr;
    inline static Color clearColor = { 1, 1, 1, 1 };

    static void init();
    static void free();

};


#endif //CONNECT_GFX_HPP
