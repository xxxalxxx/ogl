#include "Engine.h"

Engine::Engine(): mCamera(mWindow.getAspect())
{

}



Engine::~Engine()
{

}


bool Engine::init()
{

    if(!mWindow.init()) return false;

    glewExperimental = GL_TRUE;
   
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return false;
    }


    glfwSetWindowUserPointer(mWindow.getGLFWwindow(), this);

    glViewport(0, 0, mWindow.getWidth(), mWindow.getHeight());
 
    return true;
}

Engine* Engine::getEngineInstance(GLFWwindow* window)
{
    return static_cast<Engine*>(glfwGetWindowUserPointer(window));
}

