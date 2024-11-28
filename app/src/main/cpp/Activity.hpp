//
// Created by mecha on 28.11.2024.
//

#ifndef CONNECT_ACTIVITY_HPP
#define CONNECT_ACTIVITY_HPP

#include <poll.h>
#include <pthread.h>
#include <sched.h>
#include <string>
#include <cstdint>
#include <cerrno>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/input.h>
#include <android/looper.h>
#include <android/native_window.h>
#include <android/native_activity.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Connect", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, "Connect", __VA_ARGS__)

class App;

class Activity {

public:
    virtual void onCreate(void* savedState, size_t savedStateSize);
    virtual void onStart();
    virtual void onResume();
    virtual void onSaveInstanceState(size_t* outSize);
    virtual void onPause();
    virtual void onStop();
    virtual void onDestroy();
    virtual void onWindowFocusChanged(int hasFocus);
    virtual void onNativeWindowCreated(ANativeWindow* window);
    virtual void onNativeWindowResized(ANativeWindow* window);
    virtual void onNativeWindowRedrawNeeded(ANativeWindow* window);
    virtual void onNativeWindowDestroyed(ANativeWindow* window);
    virtual void onInputQueueCreated(AInputQueue* queue);
    virtual void onInputQueueDestroyed(AInputQueue* queue);
    virtual void onContentRectChanged(const ARect* rect);
    virtual void onConfigurationChanged();
    virtual void onLowMemory();
    virtual void setWindowFormat(int32_t format);
    virtual void setWindowFlags(uint32_t addFlags, uint32_t removeFlags);
    virtual void finish();
    virtual void showSoftInput(uint32_t flags);
    virtual void hideSoftInput(uint32_t flags);

private:
    App* app;
    ANativeActivity* nativeActivity = nullptr;
};

#endif //CONNECT_ACTIVITY_HPP
