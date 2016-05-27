#include "Engine.h"

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);  
    }
    else if(key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        std::cout<< "E PRESSED" <<std::endl;
    }
    else if(key == GLFW_KEY_E && action ==GLFW_REPEAT)
    {
        std::cout<< "E REPEAT" <<std::endl;
    }
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    Camera& engCamera = Engine::getEngineInstance(window)->getCamera();
    engCamera.updateOrientation(xpos, ypos);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        std::cout<< "MBR ";
        switch(action)
        {
            case GLFW_RELEASE:
                  std::cout<< "RELEASE";

                break;
            case GLFW_PRESS:  
                std::cout<< "PRESS";

                break;
            case GLFW_REPEAT:
                  std::cout<< "REPEAT";
                break;
        }
        std::cout<<std::endl;
    }
    else if(button == GLFW_MOUSE_BUTTON_LEFT)
    {
        std::cout<<"MBL";
        switch(action)
        {
            case GLFW_RELEASE:
                  std::cout<< "RELEASE";
                break;
            case GLFW_PRESS:  
                std::cout<< "PRESS";
                break;
            case GLFW_REPEAT:
                  std::cout<< "REPEAT";
                break;
        }
        std::cout<<std::endl;
    }  
}




void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    Camera& engCamera = Engine::getEngineInstance(window)->getCamera();
    engCamera.updateFOV(yoffset);
    engCamera.rebuildPerspective();
    std::cout<< "SCROLL POS " << xoffset << " " << yoffset << std::endl;
}


void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    Engine* engine = Engine::getEngineInstance(window);
    Window& engWindow = engine->getWindow();
    Camera& engCamera = engine->getCamera();
    engWindow.update(width, height);
    engCamera.setAspect(engWindow.getAspect());
    engCamera.rebuildPerspective();

    glViewport(0, 0, width, height);
}



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
    
    setKeyCallback(keyCallback);
    setCursorPosCallback(cursorPosCallback);
    setMouseButtonCallback(mouseButtonCallback);
    setScrollCallback(scrollCallback);
    setFramebufferSizeCallback(framebufferSizeCallback);
   
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



