//
// Created by cheerwizard on 30.11.24.
//

#include "Android.hpp"
#include "Application.hpp"
#include "Types.hpp"

void android_main(android_app* state) {
    app = state;
    stringstream logFilepath;
    logFilepath << app->activity->internalDataPath << "/" << "ConnectLog.log";
    Application* application = new Application(logFilepath.str().c_str());
    application->run();
    delete application;
}