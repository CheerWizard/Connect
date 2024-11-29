//
// Created by cheerwizard on 28.11.24.
//

#include "Application.hpp"
#include "Logger.hpp"

#include <android_native_app_glue.h>
#include <android/choreographer.h>

void android_main(android_app* app) {
    Application* application = new Application(app);
    application->run();
    delete application;
}

Application::Application(android_app* app)
:
app(app)
{
    app->userData = this;
    app->onAppCmd = handleCommand;
    app->onInputEvent = handleInput;

    if (app->savedState != nullptr) {
        state = *(SavedState*)app->savedState;
    }

    input = new Input(this);
}

Application::~Application() {
    delete display;
    delete input;
}

void Application::run() {
    while (!app->destroyRequested) {
        android_poll_source* source = nullptr;

        auto result = ALooper_pollOnce(
                -1,
                nullptr,
                nullptr,
                reinterpret_cast<void**>(&source)
        );

        if (result == ALOOPER_POLL_ERROR) {
            fatal("ALooper_pollOnce returned an error");
        }

        if (source != nullptr) {
            source->process(app, source);
        }
    }
}

void Application::resume() {
    // Checked to make sure we don't double schedule Choreographer.
    if (!running) {
        running = true;
        scheduleNextTick();
    }
}

void Application::pause() {
    running = false;
}

void Application::handleCommand(android_app* app, int32_t command) {
    reinterpret_cast<Application*>(app->userData)->onHandleCommand(command);
}

void Application::onHandleCommand(int32_t command) {
    switch (command) {
        case APP_CMD_SAVE_STATE:
            app->savedState = &state;
            app->savedStateSize = sizeof(SavedState);
            break;
        case APP_CMD_INIT_WINDOW:
            if (display == nullptr || app->window != nullptr) {
                display = new Display(this);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            delete display;
            pause();
            break;
        case APP_CMD_GAINED_FOCUS:
            input->onFocusGained();
            resume();
            break;
        case APP_CMD_LOST_FOCUS:
            input->onFocusLost();
            pause();
            break;
        default:
            break;
    }
}

int32_t Application::handleInput(android_app* app, AInputEvent* inputEvent) {
    return reinterpret_cast<Application*>(app->userData)->onHandleInput(inputEvent);
}

int32_t Application::onHandleInput(AInputEvent* inputEvent) {
    return input->handle(inputEvent);
}

void Application::scheduleNextTick() {
    AChoreographer_postFrameCallback(AChoreographer_getInstance(), tick, this);
}

void Application::tick(long, void *data) {
    CHECK_NOT_NULL(data);
    reinterpret_cast<Application*>(data)->onTick();
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
    state.angle += .01f;

    if (state.angle > 1) {
        state.angle = 0;
    }

    display->graphics->clearColor = {
            (float) state.x / (float) display->width,
            state.angle,
            (float) state.y / (float) display->height,
            1
    };
}

void Application::onRender() {
    if (display == nullptr) {
        return;
    }
    display->render();
    display->swap();
}

void Application::onSensor(int fd, int events) {

}

void Application::onMotionPosition(float x, float y) {
    LOGI("onMotionPosition(X:%f,Y:%f)", x, y);
    state.x = x;
    state.y = y;
}
