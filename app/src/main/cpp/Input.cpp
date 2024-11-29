//
// Created by cheerwizard on 28.11.24.
//

#include "Input.hpp"
#include "Application.hpp"
#include "Logger.hpp"

#include <android_native_app_glue.h>

static char packageName[64] = {0};

static const char* getPackageName() {
    if (packageName[0] != 0) return packageName;

    char path[64] = {0};
    sprintf(path, "/proc/self/cmdline");

    FILE* cmdline = fopen(path, "r");

    if (cmdline != nullptr) {
        fread(packageName, sizeof(packageName), 1, cmdline);
        LOGI("package=%s\n", packageName);
        fclose(cmdline);
    }

    return packageName;
}

Input::Input(Application* application)
: application(application)
{
    sensorManager = ASensorManager_getInstanceForPackage(getPackageName());

    if (sensorManager == nullptr) {
        LOGE("Failed to initialize SensorManager!");
        return;
    }

    accelerometerSensor = ASensorManager_getDefaultSensor(
            sensorManager,
            ASENSOR_TYPE_ACCELEROMETER
    );

    sensorEventQueue = ASensorManager_createEventQueue(
            sensorManager,
            application->app->looper,
            ALOOPER_POLL_CALLBACK,
            handleSensor,
            this
    );
}

Input::~Input() {

}

int32_t Input::handle(AInputEvent* inputEvent) {
    switch (AInputEvent_getType(inputEvent)) {
        case AINPUT_EVENT_TYPE_CAPTURE:
            break;
        case AINPUT_EVENT_TYPE_DRAG:
            break;
        case AINPUT_EVENT_TYPE_FOCUS:
            break;
        case AINPUT_EVENT_TYPE_KEY:
            break;
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

int Input::handleSensor(int fd, int events, void* data) {
    return reinterpret_cast<Input*>(data)->onHandleSensor(fd, events);
}

int Input::onHandleSensor(int fd, int events) {
    CHECK_NOT_NULL(accelerometerSensor);

    ASensorEvent event;
    while (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0) {
        LOGI("Accelerometer: x=%f y=%f z=%f", event.acceleration.x, event.acceleration.y, event.acceleration.z);
    }

    // Implementations should return 1 to continue receiving callbacks, or 0 to
    // have this file descriptor and callback unregistered from the looper.
    return 1;
}