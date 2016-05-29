#include "Window.h"

bool Window::init()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE); 
    mGLFWwindow = glfwCreateWindow(mWidth, mHeight, "MyEngine", nullptr, nullptr);  

    if (mGLFWwindow == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwSetInputMode(mGLFWwindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    glfwSetInputMode(mGLFWwindow, GLFW_STICKY_KEYS, 1);

    glfwMakeContextCurrent(mGLFWwindow); 

    return true;
}
