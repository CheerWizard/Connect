//
// Created by cheerwizard on 28.11.24.
//

#include "Application.hpp"
#include "Input.hpp"
#include "Window.hpp"
#include "AssetManager.hpp"
#include "Audio.hpp"
#include "Rendering.hpp"

Application::Application(const char* logFilepath) {
    LOG_INIT("Connect", logFilepath);
    initPlatform();
    restoreStatePlatform();
    Input::init();
    Audio::init();
}

Application::~Application() {
    Audio::free();
    Input::free();
    freePlatform();
    LOG_FREE();
}

void Application::run() {
    while (isPlatformAlive()) {
        pollPlatform();
    }
}

void Application::resume() {
    if (!running) {
        running = true;
        scheduleNextTick();
    }
}

void Application::pause() {
    running = false;
}

void Application::onTick() {
    if (!running) {
        return;
    }
    scheduleNextTick();
    onUpdate();
    onRender();
}

void Application::onUpdate() {
    savedState.angle += .01f;

    if (savedState.angle > 1) {
        savedState.angle = 0;
    }

    Rendering::clearColor = {
            (float) savedState.x / (float) Window::width,
            savedState.angle,
            (float) savedState.y / (float) Window::height,
            1
    };
}

void Application::onRender() {
    Rendering::render();
}

void Application::onSensor(int fd, int events) {

}

void Application::onMotionPosition(float x, float y) {
    LOG_INFO("onMotionPosition(X:%f;Y:%f)", x, y);
    savedState.x = x;
    savedState.y = y;
}

void Application::initDisplay() {
    Window::init();
    Rendering::init();
}

void Application::terminateDisplay() {
    Rendering::free();
    Window::free();
    pause();
}

void Application::focusGained() {
    Input::onFocusGained();
    resume();
}

void Application::focusLost() {
    Input::onFocusLost();
    pause();
}
