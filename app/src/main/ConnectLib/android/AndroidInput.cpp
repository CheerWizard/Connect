//
// Created by cheerwizard on 30.11.24.
//

#include "Input.hpp"
#include "Logger.hpp"
#include "Application.hpp"
#include "AndroidLauncher.hpp"

ASensorManager* sensorManager = nullptr;
const ASensor* accelerometerSensor = nullptr;
ASensorEventQueue* sensorEventQueue = nullptr;

static char packageName[64] = {0};

static const char* getPackageName() {
    if (packageName[0] != 0) return packageName;

    char path[64] = {0};
    sprintf(path, "/proc/self/cmdline");

    FILE* cmdline = fopen(path, "r");

    if (cmdline != nullptr) {
        fread(packageName, sizeof(packageName), 1, cmdline);
        LOG_INFO("package=%s\n", packageName);
        fclose(cmdline);
    }

    return packageName;
}

static int handleSensor(int fd, int events, void* data) {
    Application* application = reinterpret_cast<Application*>(app->userData);
    ASensorEvent event;
    while (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0) {
        LOG_INFO("Accelerometer: x=%f y=%f z=%f", event.acceleration.x, event.acceleration.y, event.acceleration.z);
    }
    // Implementations should return 1 to continue receiving callbacks, or 0 to
    // have this file descriptor and callback unregistered from the looper.
    return 1;
}

static int32_t handleInput(android_app* app, AInputEvent* inputEvent) {
    Application* application = reinterpret_cast<Application*>(app->userData);
    switch (AInputEvent_getType(inputEvent)) {
        case AINPUT_EVENT_TYPE_CAPTURE:
        case AINPUT_EVENT_TYPE_DRAG:
        case AINPUT_EVENT_TYPE_FOCUS:
        case AINPUT_EVENT_TYPE_KEY:
        case AINPUT_EVENT_TYPE_MOTION:
        {
            float x = AMotionEvent_getX(inputEvent, 0);
            float y = AMotionEvent_getY(inputEvent, 0);
            application->onMotionPosition(x, y);
        }
            break;
        case AINPUT_EVENT_TYPE_TOUCH_MODE:
            break;
    }
    return 1;
}

void Input::init() {
    app->onInputEvent = handleInput;

    sensorManager = ASensorManager_getInstanceForPackage(getPackageName());

    if (sensorManager == nullptr) {
        LOG_ERR("Failed to initialize SensorManager!");
        return;
    }

    accelerometerSensor = ASensorManager_getDefaultSensor(
            sensorManager,
            ASENSOR_TYPE_ACCELEROMETER
    );

    sensorEventQueue = ASensorManager_createEventQueue(
            sensorManager,
            app->looper,
            ALOOPER_POLL_CALLBACK,
            handleSensor,
            nullptr
    );
}

void Input::free() {

}

void Input::onFocusGained() {
    if (accelerometerSensor != nullptr) {
        ASensorEventQueue_enableSensor(sensorEventQueue, accelerometerSensor);
        ASensorEventQueue_setEventRate(sensorEventQueue, accelerometerSensor, (1000L / 60) * 1000);
    }
}

void Input::onFocusLost() {
    if (accelerometerSensor != nullptr) {
        ASensorEventQueue_disableSensor(sensorEventQueue, accelerometerSensor);
    }
}