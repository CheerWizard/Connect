//
// Created by cheerwizard on 28.11.24.
//

#include "Application.hpp"
#include "Logger.hpp"
#include "Input.hpp"
#include "Display.hpp"
#include "Gfx.hpp"
#include "AssetManager.hpp"

Application::Application(const char* logFilepath) {
    LOG_OPEN(logFilepath);
    initPlatform();
    restoreStatePlatform();
    Input::init();
}

Application::~Application() {
    Input::free();
    freePlatform();
    LOG_CLOSE();
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

    Gfx::clearColor = {
            (float) savedState.x / (float) Display::width,
            savedState.angle,
            (float) savedState.y / (float) Display::height,
            1
    };
}

void Application::onRender() {
    Gfx::clearBuffer(COLOR_BUFFER_BIT);
    Display::swapChain();
}

void Application::onSensor(int fd, int events) {

}

void Application::onMotionPosition(float x, float y) {
    LOG_INFO("onMotionPosition(X:%f;Y:%f)", x, y);
    savedState.x = x;
    savedState.y = y;
}

void Application::initDisplay() {
    Display::init();
    Gfx::init();
}

void Application::terminateDisplay() {
    Gfx::free();
    Display::terminate();
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
