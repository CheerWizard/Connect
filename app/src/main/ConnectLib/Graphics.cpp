//
// Created by cheerwizard on 28.11.24.
//

#include "Graphics.hpp"

void Graphics::init() {
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

void Graphics::free() {

}

void Graphics::render() {
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
    for (Shader* shader : shaders) {
        shader->run();
    }
}