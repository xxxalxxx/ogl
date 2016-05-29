#ifndef ALM_WINDOW_H
#define ALM_WINDOW_H
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define INITIAL_WINDOW_WIDTH 800
#define INITIAL_WINDOW_HEIGHT 600
#include "Utils.h"
class Window
{
public:
    Window();
    ~Window(); 
    GLFWwindow* getGLFWwindow();
    bool init(); 
    bool isOpen();

    int getHeight();
    int getWidth();
    float getAspect();

    void setHeight(int height);
    void setWidth(int width);  

    void update(int width, int height);
private:
    int mWidth, mHeight;
    GLFWwindow* mGLFWwindow;
};

inline Window::Window(): mWidth(INITIAL_WINDOW_WIDTH), mHeight(INITIAL_WINDOW_HEIGHT), mGLFWwindow(nullptr)
{
    
}

inline Window::~Window()
{
    glfwTerminate();
}

inline bool Window::isOpen()
{
    return !glfwWindowShouldClose(mGLFWwindow);
}

inline int Window::getHeight()
{
    return mHeight;
}

inline int Window::getWidth()
{
    return mWidth;
}

inline float Window::getAspect()
{
    return (float) mWidth/mHeight;
}

inline void Window::setWidth(int width)
{
   mWidth = width; 
}

inline void Window::setHeight(int height)
{
    mHeight = height;
}

inline void Window::update(int width, int height)
{
    setWidth(width);
    setHeight(height);
}

inline GLFWwindow* Window::getGLFWwindow()
{
    return mGLFWwindow;
}

#endif
