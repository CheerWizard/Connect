//
// Created by cheerwizard on 28.11.24.
//

#ifndef CONNECT_INPUT_HPP
#define CONNECT_INPUT_HPP

#include <android/sensor.h>
#include <android/input.h>

class Application;

class Input {

public:
    Input(Application* application);
    ~Input();

    int32_t handle(AInputEvent* inputEvent);

    void onFocusGained();
    void onFocusLost();

private:
    static int handleSensor(int fd, int events, void* data);
    int onHandleSensor(int fd, int events);

    Application* application;
    ASensorManager* sensorManager = nullptr;
    const ASensor* accelerometerSensor = nullptr;
    ASensorEventQueue* sensorEventQueue = nullptr;
};

#endif //CONNECT_INPUT_HPP
