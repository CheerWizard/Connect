//
// Created by cheerwizard on 28.11.24.
//

#include "Graphics.hpp"
#include "Logger.hpp"
#include "Display.hpp"

Graphics::Graphics(Application* application, Display* display)
:
display(display)
{
    context = new RenderContext(application);
}

Graphics::~Graphics() {
    delete context;
}

void Graphics::render() {
    LOGI("clearColor(%f, %f, %f, %f)", clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    context->clearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
}