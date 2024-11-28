//
// Created by cheerwizard on 28.11.24.
//

#include "Graphics.hpp"
#include "Logger.hpp"
#include "Display.hpp"

Graphics::Graphics(const Display* const display)
:
display(display)
{
    context = eglCreateContext(display->display, display->config, nullptr, nullptr);

    if (eglMakeCurrent(display->display, display->surface, display->surface, context) == EGL_FALSE) {
        LOGE("Unable to eglMakeCurrent");
        return;
    }

    auto opengl_info = { GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS };
    for (auto name : opengl_info) {
        auto info = glGetString(name);
        LOGI("OpenGL Info: %s", info);
    }

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_DEPTH_TEST);
}

Graphics::~Graphics() {
    eglMakeCurrent(display->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (context != EGL_NO_CONTEXT) {
        eglDestroyContext(display->display, context);
    }
    context = EGL_NO_CONTEXT;
}

void Graphics::render() {
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
}