//
// Created by mecha on 28.11.2024.
//

#ifndef CONNECT_APP_HPP
#define CONNECT_APP_HPP

#include "Activity.hpp"

enum LooperID {
    /**
     * Looper data ID of commands coming from the app's main thread.
     * These can be retrieved and processed with android_app_read_cmd()
     * and android_app_exec_cmd().
     */
    MAIN = 1,
    /**
     * Looper data ID of events coming from the AInputQueue of the
     * application's window.  These can be read via the inputQueue
     * object of android_app.
     */
    EVENT = 2
};

enum AppCommand {
    /**
     * Command from main thread: the AInputQueue has changed.  Upon processing
     * this command, android_app->inputQueue will be updated to the new queue
     * (or NULL).
     */
    INPUT_CHANGED,
    /**
     * Command from main thread: the ANativeWindow has changed.  Upon processing
     * this command, android_app->window will be updated to the new window surface
     * (or NULL).
     */
    WINDOW_CHANGED,
    /**
     * Command from main thread: the current ANativeWindow has been resized.
     * Please redraw with its new size.
     */
    WINDOW_RESIZED,
    /**
     * Command from main thread: the system needs that the current ANativeWindow
     * be redrawn.  You should redraw the window before handing this to
     * android_app_exec_cmd() in order to avoid transient drawing glitches.
     */
    WINDOW_REDRAW_NEEDED,
    /**
     * Command from main thread: the content area of the window has changed,
     * such as from the soft input window being shown or hidden.  You can
     * find the new content rect in android_app::contentRect.
     */
    CONTENT_RECT_CHANGED,
    /**
     * Command from main thread: the app's activity window has gained
     * input focus.
     */
    GAINED_FOCUS,
    /**
     * Command from main thread: the app's activity window has lost
     * input focus.
     */
    LOST_FOCUS,
    /**
     * Command from main thread: the system is running low on memory.
     * Try to reduce your memory use.
     */
    LOW_MEMORY,
    /**
     * Command from main thread: the app's activity has been started.
     */
    START,
    /**
     * Command from main thread: the app's activity has been resumed.
     */
    RESUME,
    /**
     * Command from main thread: the app's activity has been paused.
     */
    PAUSE,
    /**
     * Command from main thread: the app's activity has been stopped.
     */
    STOP,
    /**
     * Command from main thread: the app's activity is being destroyed,
     * and waiting for the app thread to clean up and exit before proceeding.
     */
    DESTROY,
};

class App {

public:
    // The Activity object instance that this app is running in.
    Activity* activity = nullptr;

    App(Activity* activity);
    ~App();

    void* run();
    int32_t handleCommand(int8_t cmd);
    int8_t readCommand() const;
    void writeCommand(int8_t command);
    void setInput(AInputQueue* inputQueue);
    void setWindow(ANativeWindow* window);
    void setActivityState(int8_t command);

private:
    void onDestroy();

private:
    // The application can place a pointer to its own state object
    // here if it likes.
    void* userData = nullptr;
    // The ALooper associated with the app's thread.
    ALooper* looper = nullptr;
    // When non-NULL, this is the input queue from which the app will
    // receive user input events.
    AInputQueue* inputQueue = nullptr;
    // When non-NULL, this is the window surface that the app can draw in.
    ANativeWindow* window = nullptr;
    // Current content rectangle of the window; this is the area where the
    // window's content should be placed to be seen by the user.
    ARect contentRect;
    // Current state of the app's activity.  May be either APP_CMD_START,
    // APP_CMD_RESUME, APP_CMD_PAUSE, or APP_CMD_STOP; see below.
    int activityState;
    // -------------------------------------------------
    // Below are "private" implementation of the glue code.
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int msgread;
    int msgwrite;
    pthread_t thread;
    // This is non-zero when the application's NativeActivity is being
    // destroyed and waiting for the app thread to complete.
    int destroyRequested;
    int running;
    int destroyed;
    int redrawNeeded;
    AInputQueue* pendingInputQueue = nullptr;
    ANativeWindow* pendingWindow = nullptr;
    ARect pendingContentRect;
};

#endif //CONNECT_APP_HPP
