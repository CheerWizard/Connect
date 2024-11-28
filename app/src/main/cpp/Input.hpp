//
// Created by cheerwizard on 28.11.24.
//

#ifndef CONNECT_INPUT_HPP
#define CONNECT_INPUT_HPP

#include <vector>
#include <android/sensor.h>
#include <android/input.h>

class Application;

class InputCallback {
public:
    virtual void onSensor(int fd, int events) = 0;
    virtual void onMotionPosition(float x, float y) = 0;
};

class Input {

public:
    Input(const Application* const application);
    ~Input();

    void registerCallback(InputCallback* callback);
    void unregisterCallback(InputCallback* callback);

    int32_t handle(AInputEvent* inputEvent);

    void onFocusGained();
    void onFocusLost();

private:
    static int handleSensor(int fd, int events, void* data);
    int onHandleSensor(int fd, int events);

    std::vector<InputCallback*> callbacks;
    ASensorManager* sensorManager = nullptr;
    const ASensor* accelerometerSensor = nullptr;
    ASensorEventQueue* sensorEventQueue = nullptr;

};

#endif //CONNECT_INPUT_HPP
