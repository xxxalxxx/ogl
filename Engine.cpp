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

void Engine::handleCameraMovement(float dt)
{
    Camera& camera = getCamera();

    if(keyIsPressed(GLFW_KEY_W))
        camera.moveStraight(1.0f, dt);
    if(keyIsPressed(GLFW_KEY_S))
        camera.moveStraight(-1.0f, dt);

    if(keyIsPressed(GLFW_KEY_D))
        camera.moveSideways(1.0f, dt);
    if(keyIsPressed(GLFW_KEY_A))
        camera.moveSideways(-1.0f, dt);
}

bool Engine::keyIsPressed(int key)
{
    return glfwGetKey(mWindow.getGLFWwindow(), key) == GLFW_PRESS;
}

Engine* Engine::getEngineInstance(GLFWwindow* window)
{
    return static_cast<Engine*>(glfwGetWindowUserPointer(window));
}

