//
// Created by cheerwizard on 02.12.24.
//

#ifndef CONNECT_SHADER_HPP
#define CONNECT_SHADER_HPP

#include "Types.hpp"
#include "Geometry.hpp"

struct ShaderSource {
    string filepath;
    string src;
};

struct ShaderVar {

    struct Texture {
        uint32_t id;
    };

    struct Uniform {
        uint32_t id;
    };

    struct UniformBuffer {
        uint32_t id;
    };

};

class Shader {

public:
    Shader();
    ~Shader();

    void run();

private:
    AttributeLayout attributeLayout = Vertex::Attribute;
};

#endif //CONNECT_SHADER_HPP