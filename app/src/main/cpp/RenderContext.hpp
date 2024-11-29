//
// Created by cheerwizard on 29.11.24.
//

#ifndef CONNECT_RENDERCONTEXT_HPP
#define CONNECT_RENDERCONTEXT_HPP

class Application;

class RenderContext {

public:
    RenderContext(Application* application);
    ~RenderContext();

    void clearColor(float r, float g, float b, float a);

};


#endif //CONNECT_RENDERCONTEXT_HPP
