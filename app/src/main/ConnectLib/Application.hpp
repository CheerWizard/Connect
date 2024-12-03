//
// Created by cheerwizard on 28.11.24.
//

#ifndef CONNECT_APPLICATION_HPP
#define CONNECT_APPLICATION_HPP

struct SavedState {
    float angle = 0.0f;
    int32_t x = 0;
    int32_t y = 0;
};

class Application {

public:
    Application(const char* logFilepath);
    ~Application();

    void run();
    void resume();
    void pause();

    void onSensor(int fd, int events);
    void onMotionPosition(float x, float y);

    void initDisplay();
    void terminateDisplay();

    void focusGained();
    void focusLost();

    void saveStatePlatform();

private:
    void initPlatform();
    void freePlatform();
    bool isPlatformAlive();
    void pollPlatform();
    void restoreStatePlatform();

    void scheduleNextTick();

    static void tick(long, void* data);

    void onTick();

    void onUpdate();

    void onRender();

    bool running = false;
    SavedState savedState;
};

#endif //CONNECT_APPLICATION_HPP
