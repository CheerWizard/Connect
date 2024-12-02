//
// Created by cheerwizard on 02.12.24.
//

#ifndef CONNECT_ATTRIBUTELAYOUT_HPP
#define CONNECT_ATTRIBUTELAYOUT_HPP

#include "Types.hpp"

enum AttributeType
{
    AttributeType_FLOAT,          // 4 bytes
    AttributeType_BOOL,           // 4 bytes
    AttributeType_INT,            // 4 bytes
    AttributeType_UINT,           // 4 bytes

    AttributeType_VEC2_FLOAT,     // 8 bytes
    AttributeType_VEC2_UINT,      // 8 bytes
    AttributeType_VEC2_INT,       // 8 bytes
    AttributeType_VEC2_TYPELESS,  // 8 bytes

    AttributeType_VEC3_FLOAT,     // 12 bytes
    AttributeType_VEC3_UINT,      // 12 bytes
    AttributeType_VEC3_INT,       // 12 bytes
    AttributeType_VEC3_TYPELESS,  // 12 bytes

    AttributeType_VEC4_FLOAT,     // 16 bytes
    AttributeType_VEC4_UINT,      // 16 bytes
    AttributeType_VEC4_INT,       // 16 bytes
    AttributeType_VEC4_TYPELESS,  // 16 bytes

    AttributeType_MAT2X2,         // 16 bytes
    AttributeType_MAT3X3,         // 36 bytes
    AttributeType_MAT4X4,         // 64 bytes
};

struct Attribute {
    uint32_t id = 0;
    uint32_t location = 0;
    AttributeType type;
    uint8_t byteSize = 0;
};

struct AttributeLayout {
    vector<Attribute> attributes;
    size_t stride = 0;

    AttributeLayout(const vector<Attribute>& attributes) : attributes(attributes) {
        for (const auto& attribute : attributes) {
            stride += attribute.byteSize;
        }
    }

    void bind();
    void unbind();
};

// All possible vertex attributes
struct Attributes
{
    static constexpr Attribute POS = { 0, 0, AttributeType_VEC2_FLOAT, 8 };
    static constexpr Attribute UV = { 0, 1, AttributeType_VEC2_FLOAT, 8 };
};


#endif //CONNECT_ATTRIBUTELAYOUT_HPP
