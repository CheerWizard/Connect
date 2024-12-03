//
// Created by cheerwizard on 02.12.24.
//

#ifndef CONNECT_GRAPHICSCORE_HPP
#define CONNECT_GRAPHICSCORE_HPP

static inline constexpr uint32_t INVALID_RESOURCE_ID = UINT32_MAX;

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

struct Viewport {
    float left = 0;
    float top = 0;
    float width = 0;
    float height = 0;
    float minDepth = 0;
    float maxDepth = 1;
};

struct Texture {
    uint32_t id = INVALID_RESOURCE_ID;
};

struct Vertex {
    inline static AttributeLayout layout = {{
                                                    Attributes::POS,
                                                    Attributes::UV
                                            }};

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

struct ShaderSource {
    string filepath;
    string src;
};

struct Uniform {
    uint32_t id = INVALID_RESOURCE_ID;
    const char* name = nullptr;
    float* data = nullptr;
};

struct UniformBuffer {
    uint32_t id;
};

class Shader {

public:
    Shader();
    virtual ~Shader();

    void run();

private:
    AttributeLayout attributeLayout = Vertex::layout;
    vector<Uniform> uniforms;
    vector<UniformBuffer> uniformBuffers;
    vector<Texture> textures;
};

#endif //CONNECT_GRAPHICSCORE_HPP