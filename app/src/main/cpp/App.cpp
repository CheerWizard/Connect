//
// Created by mecha on 28.11.2024.
//

#include "App.hpp"

App::App(Activity* activity) {
    this->activity = activity;
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cond, nullptr);
    int msgpipe[2];
    if (pipe(msgpipe)) {
        LOGI("could not create pipe: %s", strerror(errno));
    }
    msgread = msgpipe[0];
    msgwrite = msgpipe[1];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&thread, &attr, run, this);
    // Wait for thread to start.
    pthread_mutex_lock(&android_app->mutex);
    while (!android_app->running) {
        pthread_cond_wait(&android_app->cond, &android_app->mutex);
    }
    pthread_mutex_unlock(&android_app->mutex);
    return android_app;
}

App::~App() {
    LOGI("App::~App()");
    pthread_mutex_lock(&mutex);
    if (inputQueue != nullptr) {
        AInputQueue_detachLooper(inputQueue);
    }
    destroyed = 1;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    // Can't touch android_app object after this.
    onDestroy();
}

void* App::run() {
    looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    ALooper_addFd(looper, msgread, POLLIN, nullptr, (void*) LooperID::MAIN);
    pthread_mutex_lock(&mutex);
    running = 1;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    run();
}

int32_t App::handleCommand(int8_t cmd) {
    LOGI("handleCommand(%i)", cmd);
    switch (cmd) {
        case AppCommand::INPUT_CHANGED:
            LOGI("AppCommand::INPUT_CHANGED\n");
            pthread_mutex_lock(&mutex);
            if (inputQueue != nullptr) {
                AInputQueue_detachLooper(inputQueue);
            }
            inputQueue = pendingInputQueue;
            if (inputQueue != nullptr) {
                LOGI("Attaching input queue to looper");
                AInputQueue_attachLooper(inputQueue, looper, nullptr, (void*) LooperID::EVENT);
            }
            pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&mutex);
            break;
        case AppCommand::WINDOW_CHANGED:
            LOGI("AppCommand::WINDOW_CHANGED\n");
            pthread_mutex_lock(&mutex);
            window = pendingWindow;
            pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&mutex);
            break;
        case AppCommand::START:
        case AppCommand::RESUME:
        case AppCommand::PAUSE:
        case AppCommand::STOP:
            LOGI("activityState=%d\n", cmd);
            pthread_mutex_lock(&mutex);
            activityState = cmd;
            pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&mutex);
            break;
        case AppCommand::DESTROY:
            LOGI("AppCommand::DESTROY\n");
            destroyRequested = 1;
            break;
    }
    return destroyRequested ? 0 : 1;
}

int8_t App::readCommand() const {
    LOGI("App::readCommand()")
    int8_t cmd;
    if (read(msgread, &cmd, sizeof(cmd)) == sizeof(cmd)) {
        return cmd;
    } else {
        LOGW("No data on command pipe!");
    }
    return -1;
}

void App::writeCommand(int8_t command) {
    if (write(msgwrite, &command, sizeof(command)) != sizeof(command)) {
        LOGI("Failure writing App command: %s\n", strerror(errno));
    }
}

void App::setInput(AInputQueue* inputQueue) {
    pthread_mutex_lock(&mutex);
    pendingInputQueue = inputQueue;
    writeCommand(AppCommand::INPUT_CHANGED);
    while (inputQueue != pendingInputQueue) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}

void App::setWindow(ANativeWindow *window) {
    pthread_mutex_lock(&mutex);
    pendingWindow = window;
    writeCommand(AppCommand::WINDOW_CHANGED);
    while (window != pendingWindow) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}

void App::setActivityState(int8_t command) {
    pthread_mutex_lock(&mutex);
    writeCommand(command);
    while (activityState != command) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}

void App::onDestroy() {
    pthread_mutex_lock(&mutex);
    writeCommand(AppCommand::DESTROY);
    while (!destroyed) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);
    close(msgread);
    close(msgwrite);
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
}