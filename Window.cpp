#include "Window.h"
#include "Window.h"

Window::Window(): mWidth(INITIAL_WINDOW_WIDTH), mHeight(INITIAL_WINDOW_HEIGHT)
{
}



Window::~Window()
{
    glfwTerminate();
}


bool Window::init()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE); 

    mWindow = glfwCreateWindow(mWidth, mHeight, "MyEngine", nullptr, nullptr);  
    
    if (mWindow == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(mWindow); 
    
    glewExperimental = GL_TRUE;
   
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return false;
    }
   
    glfwSetWindowUserPointer(mWindow, this);

    glViewport(0, 0, mWidth, mHeight);
    glfwSetFramebufferSizeCallback(mWindow, frameBufferSizeCallback);

    return true;
}


void Window::frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->mWidth = width;
    self->mHeight = height;
    
    glViewport(0, 0, width, height);
}
