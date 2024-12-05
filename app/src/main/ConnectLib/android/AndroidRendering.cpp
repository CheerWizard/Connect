//
// Created by cheerwizard on 04.12.24.
//

#include "Rendering.hpp"
#include "Window.hpp"

#include <EGL/egl.h>
#include <GLES3/gl3.h>

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

EGLContext context;
EGLDisplay display;
EGLSurface surface;
EGLint numConfigs;
EGLConfig config = nullptr;

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

void Rendering::init() {
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE,    8,
            EGL_GREEN_SIZE,   8,
            EGL_RED_SIZE,     8,
            EGL_NONE
    };

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, nullptr, nullptr);

    eglChooseConfig(display, attribs, nullptr, 0, &numConfigs);
    std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
    assert(supportedConfigs);
    eglChooseConfig(display, attribs, supportedConfigs.get(), numConfigs,
                    &numConfigs);
    assert(numConfigs);
    auto i = 0;
    for (; i < numConfigs; i++) {
        auto& cfg = supportedConfigs[i];
        EGLint r, g, b, d;
        if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r) &&
            eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) &&
            eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b) &&
            eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) && r == 8 &&
            g == 8 && b == 8 && d == 0) {
            config = supportedConfigs[i];
            break;
        }
    }
    if (i == numConfigs) {
        config = supportedConfigs[0];
    }

    if (config == nullptr) {
        LOG_WARN("Unable to initialize EGLConfig");
    }

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &Window::format);
    surface = eglCreateWindowSurface(display, config, app->window, nullptr);

    /* A version of OpenGL has not been specified here.  This will default to
     * OpenGL 1.0.  You will need to change this if you want to use the newer
     * features of OpenGL like shaders. */
    context = eglCreateContext(display, config, nullptr, nullptr);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOG_WARN("Unable to eglMakeCurrent");
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &Window::width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &Window::height);

    glPrint(GL_VENDOR);
    glPrint(GL_RENDERER);
    glPrint(GL_VERSION);
    glPrint(GL_EXTENSIONS);
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}

void Rendering::free() {
    if (display != EGL_NO_DISPLAY) {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context != EGL_NO_CONTEXT) {
            eglDestroyContext(display, context);
            context = EGL_NO_CONTEXT;
        }
        if (surface != EGL_NO_SURFACE) {
            eglDestroySurface(display, surface);
        }
        eglTerminate(display);
    }
    display = EGL_NO_DISPLAY;
    surface = EGL_NO_SURFACE;
}

void Rendering::render() {
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);

    // render here

    eglSwapBuffers(display, surface);
}

void Rendering::resize(int w, int h) {
    glViewport(0, 0, w, h);
}