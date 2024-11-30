//
// Created by cheerwizard on 28.11.24.
//

#include "Gfx.hpp"
#include "Logger.hpp"

#include <EGL/egl.h>
#include <GLES/gl.h>

#define GL_CONTEXT reinterpret_cast<EGLContext>(context)

void Gfx::init() {
    const GLubyte* vendor = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* extensions = glGetString(GL_EXTENSIONS);

    LOG_INFO("OpenGL Vendor: %s", vendor);
    LOG_INFO("OpenGL Renderer: %s", renderer);
    LOG_INFO("OpenGL Version: %s", version);
    LOG_INFO("OpenGL Extensions: %s", extensions);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_DEPTH_TEST);
}

void Gfx::free() {
}

void Gfx::clearBuffer(int bufferBits) {
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(bufferBits);
}