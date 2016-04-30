#ifndef ALM_WINDOW_H
#define ALM_WINDOW_H

#define GLEW_STATIC

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Window.h"
#include "Camera.h"
#include "Timer.h"

class Engine
{
public:
    Engine();
    ~Engine();
    bool init();
    bool windowIsOpen();

    void pollEvents();
    void swapBuffers();

    float getDt();
    Camera& getCamera();
    Window& getWindow();
    Timer& getTimer();

    void setFramebufferSizeCallback(GLFWframebuffersizefun cbfun);
    void setScrollCallback(GLFWscrollfun cbfun);
    void setKeyCallback(GLFWkeyfun cbfun);
    void setCursorPosCallback(GLFWcursorposfun cbfun);
    void setMouseButtonCallback(GLFWmousebuttonfun cbfun);
    static Engine* getEngineInstance(GLFWwindow* window);

private:
    Window mWindow;
    Camera mCamera;
    Timer mTimer;


};

inline bool Engine::windowIsOpen()
{
    return mWindow.isOpen();
}

inline float Engine::getDt()
{
    return mTimer.tick();
}

inline Timer& Engine::getTimer()
{
    return mTimer;
}

inline Camera& Engine::getCamera()
{
    return mCamera;
}

inline Window& Engine::getWindow()
{
    return mWindow;
}

inline void Engine::pollEvents()
{
    glfwPollEvents();
}

inline void Engine::swapBuffers()
{
    glfwSwapBuffers(mWindow.getGLFWwindow());
}

inline void setFramebufferSizeCallback(GLFWframebuffersizefun cbfun)
{
    glfwSetFramebufferSizeCallback(mWindow.getGLFWwindow(), cbfun);
}

inline void Engine::setScrollCallback(GLFWscrollfun cbfun)
{   
     glfwSetScrollCallback(mWindow.getGLFWwindow(), cbfun);
}

inline void Engine::setKeyCallback(GLFWkeyfun cbfun)
{
    glfwSetKeyCallback(mWindow.getGLFWwindow(), cbfun); 
}

inline void Engine::setCursorPosCallback(GLFWcursorposfun cbfun)
{
    glfwSetCursorPosCallback(mWindow.getGLFWwindow(), cbfun);
}

inline void Engine::setMouseButtonCallback(GLFWmousebuttonfun cbfun)
{
    glfwSetMouseButtonCallback(mWindow.getGLFWwindow(), cbfun);
}


#endif
