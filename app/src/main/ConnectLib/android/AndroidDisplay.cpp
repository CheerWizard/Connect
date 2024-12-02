//
// Created by cheerwizard on 30.11.24.
//

#include "Display.hpp"
#include "Logger.hpp"
#include "Gfx.hpp"

#include "Android.hpp"

#include <EGL/egl.h>

EGLint numConfigs;
EGLConfig config = nullptr;
EGLDisplay display;
EGLSurface surface;

void Display::init() {
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
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    surface = eglCreateWindowSurface(display, config, app->window, nullptr);

    /* A version of OpenGL has not been specified here.  This will default to
     * OpenGL 1.0.  You will need to change this if you want to use the newer
     * features of OpenGL like shaders. */
    Gfx::context = eglCreateContext(display, config, nullptr, nullptr);

    if (eglMakeCurrent(display, surface, surface, Gfx::context) == EGL_FALSE) {
        LOG_WARN("Unable to eglMakeCurrent");
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &height);
}

void Display::terminate() {
    if (display != EGL_NO_DISPLAY) {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (Gfx::context != EGL_NO_CONTEXT) {
            eglDestroyContext(display, Gfx::context);
            Gfx::context = EGL_NO_CONTEXT;
        }
        if (surface != EGL_NO_SURFACE) {
            eglDestroySurface(display, surface);
        }
        eglTerminate(display);
    }
    display = EGL_NO_DISPLAY;
    surface = EGL_NO_SURFACE;
}

void Display::swapChain() {
    eglSwapBuffers(display, surface);
}