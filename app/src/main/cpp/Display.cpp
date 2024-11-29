//
// Created by cheerwizard on 28.11.24.
//

#include "Display.hpp"
#include "Application.hpp"
#include "Logger.hpp"

#include <android_native_app_glue.h>

Display::Display(Application* application) {
    graphics = new Graphics(application, this);
}

Display::~Display() {
    terminate();
}

void Display::swap() {
}

void Display::render() {
    graphics->render();
}

void Display::terminate() {
    delete graphics;
}