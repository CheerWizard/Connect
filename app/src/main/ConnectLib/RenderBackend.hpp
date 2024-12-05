//
// Created by cheerwizard on 02.12.24.
//

#ifndef CONNECT_RENDERBACKEND_HPP
#define CONNECT_RENDERBACKEND_HPP

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
    ID id = ID_NULL;
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
    ID id = 0;
};

struct Vertex {
    inline static AttributeLayout layout = {
    {
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

class UniformBuffer {

public:
    ID id = ID_NULL;

};

struct RenderTarget {
    ID id = ID_NULL;
};

class UniformBuffers {
};

enum ShaderStage {
    ShaderStage_VERTEX,
    ShaderStage_GEOMETRY,
    ShaderStage_TESSELATION,
    ShaderStage_PIXEL,
    ShaderStage_COMPUTE,
};

struct ShaderSource {
    ShaderStage stage;
    string source;

    ShaderSource() = default;
    ShaderSource(ShaderStage stage, const string& source)
    : stage(stage), source(source) {}
};

class Shader {

public:
    Shader(const char* filepath);
    virtual ~Shader();

private:
    string readWithIncludes(const string& filepath);
    vector<ShaderSource> readSources(const string& filepath);

    void init();
    void free();
    void bind();
    bool compile(const ShaderSource& shaderSource);
    bool link();

private:
    ID id = ID_NULL;
    AttributeLayout* attributeLayout = nullptr;
    vector<UniformBuffer*> uniformBuffers;
    vector<Texture*> textures;
    vector<RenderTarget*> renderTargets;
};

class RenderBackend {

public:
    inline static Vec4<float> clearColor = {0.5, 0.5, 0.5, 1};

    static void init();
    static void free();
    static void render();
    static void resize(int w, int h);
};

#endif //CONNECT_RENDERBACKEND_HPP