//
// Created by cheerwizard on 02.12.24.
//

#ifndef CONNECT_GEOMETRY_HPP
#define CONNECT_GEOMETRY_HPP

#include "AttributeLayout.hpp"
#include "Math.hpp"

struct Vertex {
    inline static AttributeLayout layout = {
            Attributes::POS,
            Attributes::UV
    };

    Vec2<float> pos;
    Vec2<float> uv;
};

using Index = uint32_t;

struct GeometryData {
    vector<Vertex> vertices;
    vector<Index> indices;
};

enum GeometryDrawType {
    GeometryDrawType_TRIANGLE,
    GeometryDrawType_TRIANGLE_STRIP,
    GeometryDrawType_QUAD,
    GeometryDrawType_POINT,
    GeometryDrawType_LINE,

    GeometryDrawType_DEFAULT = GeometryDrawType_TRIANGLE
};

struct GeometryDrawData {
    GeometryDrawType drawType = GeometryDrawType_DEFAULT;
    uint32_t vertexOffset = 0;
    uint32_t indexOffset = 0;
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;
};

#endif //CONNECT_GEOMETRY_HPP
