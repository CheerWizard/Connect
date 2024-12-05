//
// Created by cheerwizard on 04.12.24.
//

#include "RenderBackend.hpp"

static void printGlString(const char* name, GLenum s) {

}

#if defined(DEBUG)

#define glCall(function) \
function;                \
GLint error = glGetError(); \
ASSERT(error == GL_NO_ERROR, "GL error after %s(): 0x%08x", #function, error)

#define glPrint(v) LOG_VERB("%s: %s\n", #v, (const char*) glGetString(v))

#else

#define glCall(function) function
#define glPrint(v)

#endif

void AttributeLayout::bind() {
}

void AttributeLayout::unbind() {

}

void GeometryBuffer::init() {

}

void GeometryBuffer::free() {

}

GeometryDrawData GeometryBuffer::addGeometry(const GeometryData &geometryData) {
    return GeometryDrawData();
}

void GeometryBuffer::removeGeometry(const GeometryDrawData &geometryDrawData) {

}

void GeometryBuffer::flush() {

}

void Shader::init() {
    glCall(id = glCreateProgram());
}

void Shader::free() {
    if (id != ID_NULL) {
        glDeleteProgram(id);
        id = ID_NULL;
    }
}

static GLenum toShaderType(ShaderStage stage) {
    switch (stage) {
        case ShaderStage_VERTEX: return GL_VERTEX_SHADER;
        case ShaderStage_PIXEL: return GL_FRAGMENT_SHADER;
        case ShaderStage_GEOMETRY: return 0;
        case ShaderStage_TESSELATION: return 0;
        case ShaderStage_COMPUTE: return 0;
        default: return 0;
    }
}

static const char* toString(ShaderStage stage) {
    switch (stage) {
        case ShaderStage_VERTEX: return "vertex";
        case ShaderStage_PIXEL: return "pixel";
        case ShaderStage_GEOMETRY: return "geometry";
        case ShaderStage_TESSELATION: return "tesselation";
        case ShaderStage_COMPUTE: return "compute";
        default: return "unknown";
    }
}

bool Shader::compile(const ShaderSource &shaderSource) {
    glCall(GLuint stageId = glCreateShader(toShaderType(shaderSource.stage)));

    const char* src = shaderSource.source.c_str();
    glShaderSource(stageId, 1, &src, nullptr);

    GLint compiled = GL_FALSE;
    glCompileShader(stageId);
    glGetShaderiv(stageId, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLogLen = 0;
        glGetShaderiv(stageId, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0) {
            GLchar* infoLog = (GLchar*) std::malloc(infoLogLen);
            if (infoLog) {
                glGetShaderInfoLog(stageId, infoLogLen, nullptr, infoLog);
                LOG_ERR("Could not compile %s shader:\n%s\n", toString(shaderSource.stage), infoLog);
                std::free(infoLog);
            }
        }
        glDeleteShader(stageId);
        return false;
    }

    glAttachShader(id, stageId);

    return true;
}

bool Shader::link() {
    GLint linked = GL_FALSE;

    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint infoLogLen = 0;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen) {
            GLchar* infoLog = (GLchar*) std::malloc(infoLogLen);
            if (infoLog) {
                glGetProgramInfoLog(id, infoLogLen, nullptr, infoLog);
                LOG_ERR("Failed to link shader program:\n%s\n", infoLog);
                std::free(infoLog);
            }
        } else {
            LOG_ERR("Failed to link shader program");
        }
        return false;
    }

    return true;
}

void Shader::bind() {
    glUseProgram(id);
}

void RenderBackend::init() {
    glPrint(GL_VENDOR);
    glPrint(GL_RENDERER);
    glPrint(GL_VERSION);
    glPrint(GL_EXTENSIONS);
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}

void RenderBackend::free() {

}

void RenderBackend::render() {
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void RenderBackend::resize(int w, int h) {
    glViewport(0, 0, w, h);
}