#ifndef ALM_WINDOW_H
#define ALM_WINDOW_H

#define INITIAL_WINDOW_WIDTH 800
#define INITIAL_WINDOW_HEIGHT 600

#define GLEW_STATIC

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{
    friend struct GLFWwindow;
public:
    Window();
    ~Window();
    bool init();
    bool isOpened();
    int getHeight();
    int getWidth();
    void pollEvents();
    void swapBuffers();
private:
    static void frameBufferSizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow  *mWindow;
    int mWidth, mHeight;
};

inline bool Window::isOpened()
{
    return !glfwWindowShouldClose(mWindow);
}

inline void Window::pollEvents()
{
    glfwPollEvents();
}

inline void Window::swapBuffers()
{
    glfwSwapBuffers(mWindow);
}


inline int Window::getWidth()
{
    return mWidth;
}

inline int Window::getHeight()
{
    return mHeight;
}




#endif
