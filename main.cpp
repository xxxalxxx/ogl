#include <iostream>
#include "Engine.h"

#include "Shader.h"
#include "Quad.h"

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

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

}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    Camera& engCamera = Engine::getEngineInstance(window)->getCamera();
    engCamera.updateOrientation(xpos, ypos);
    engCamera.rebuildView();
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


int main()
{
    Engine engine;
    if(!engine.init()) return -1;
    
    engine.setKeyCallback(keyCallback);
    engine.setCursorPosCallback(cursorPosCallback);
    engine.setMouseButtonCallback(mouseButtonCallback);
    engine.setScrollCallback(scrollCallback);
    engine.setFramebufferSizeCallback(framebufferSizeCallback);

  //  Shader quadShader("Shaders/quad.vert","Shaders/quad.frag");
   // quadShader.Use();

    Shader quadShader;
    if(!quadShader.init("Shaders/quad.vert","Shaders/quad.frag")) return -1;
    quadShader.use();
    Quad quad;
    quad.init(quadShader.getProgram(), "res/img.jpg");

    glm::mat4& proj = engine.getCamera().getProj();
    glm::mat4& view = engine.getCamera().getView();

    while(engine.windowIsOpen())
    {
        float dt = engine.getDt();
    
        glm::mat4 viewProj = proj * view; 
        engine.pollEvents();
       
        glClearColor(0.0f,0.0f,0.3f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        quad.draw(viewProj, dt);
       // std::cout<< "HELLO " << window.getWidth() <<" " << window.getHeight() << std::endl;

        engine.swapBuffers();
    }

    return 0;
}






