#include <iostream>
#include "Window.h"


int main()
{
    Window window;
    if(!window.init()) return -1;

    while(window.isOpened())
    {
        window.pollEvents();

        glClearColor(1.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        std::cout<< "HELLO " << window.getWidth() <<" " << window.getHeight() << std::endl;

        window.swapBuffers();
    }
        return 0;
}
