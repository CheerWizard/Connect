//
// Created by cheerwizard on 30.11.24.
//

#include "Application.hpp"

static void handleCommand(android_app* app, int32_t command) {
    Application* application = reinterpret_cast<Application*>(app->userData);
    switch (command) {
        case APP_CMD_SAVE_STATE:
            application->saveStatePlatform();
            break;
        case APP_CMD_INIT_WINDOW:
            application->initDisplay();
            break;
        case APP_CMD_TERM_WINDOW:
            application->terminateDisplay();
            break;
        case APP_CMD_GAINED_FOCUS:
            application->focusGained();
            break;
        case APP_CMD_LOST_FOCUS:
            application->focusLost();
            break;
        default:
            break;
    }
}

void Application::initPlatform() {
    app->userData = this;
    app->onAppCmd = handleCommand;
}

void Application::freePlatform() {

}

bool Application::isPlatformAlive() {
    return !app->destroyRequested;
}

void Application::pollPlatform() {
    android_poll_source* source = nullptr;

    auto result = ALooper_pollOnce(
            -1,
            nullptr,
            nullptr,
            reinterpret_cast<void**>(&source)
    );

    if (result == ALOOPER_POLL_ERROR) {
        LOG_ERR("ALooper_pollOnce returned an error");
    }

    if (source != nullptr) {
        source->process(app, source);
    }
}

void Application::saveStatePlatform() {
    app->savedState = &savedState;
    app->savedStateSize = sizeof(SavedState);
}

void Application::restoreStatePlatform() {
    if (app->savedState != nullptr) {
        savedState = *(SavedState*)app->savedState;
    }
}

void Application::scheduleNextTick() {
    AChoreographer_postFrameCallback(AChoreographer_getInstance(), tick, nullptr);
}

void Application::tick(long, void *data) {
    reinterpret_cast<Application*>(app->userData)->onTick();
}