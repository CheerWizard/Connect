//
// Created by cheerwizard on 28.11.24.
//

#ifndef CONNECT_APPLICATION_HPP
#define CONNECT_APPLICATION_HPP

#include "Types.hpp"
#include "Input.hpp"
#include "Display.hpp"

class android_app;

/**
 * Our saved state data.
 */
struct SavedState {
    float angle = 0.0f;
    int32_t x = 0;
    int32_t y = 0;
};

class Application {

public:
    android_app* app = nullptr;

    Application(android_app* app);
    ~Application();

    void run();
    void resume();
    void pause();

    void onSensor(int fd, int events);
    void onMotionPosition(float x, float y);

private:
    static void handleCommand(android_app* app, int32_t command);

    void onHandleCommand(int32_t command);

    static int32_t handleInput(android_app* app, AInputEvent* inputEvent);

    int32_t onHandleInput(AInputEvent* inputEvent);

    void scheduleNextTick();

    static void tick(long, void* data);

    void onTick();

    void onUpdate();

    void onRender();

    bool running = false;
    Display* display = nullptr;
    Input* input = nullptr;
    SavedState state;
};

#endif //CONNECT_APPLICATION_HPP
