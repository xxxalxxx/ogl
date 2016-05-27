#include <iostream>

#include "Engine.h"
#include "FileSystem.h"
#include "Shader.h"
//#include "Quad.h"
#include "Model.h" 
#include "SkinnedModel.h"

int main()
{
    Engine engine;

    if(!engine.init()) return -1;

    FileSystem& fs = engine.getFileSystem();

    glEnable(GL_DEPTH_TEST);
/*
    Shader modelShader;
    if(!modelShader.init("Shaders/shader.vs", "Shaders/shader.frag")) return -1;
    modelShader.use();
    Model modelNanosuit(
           "res/mesh/guard/boblampclean.md5mesh", // "res/mesh/police/Police.obj",
            modelShader,
            "res/mesh/guard/"// "res/mesh/police/Texture",
            );

    if(!modelNanosuit.init()) return -1;
*/
    //abs path needed for linking shaders if bin is run form different directory
    Shader skinnedModelShader;
    if(!skinnedModelShader.init(fs.getAbsPath("Shaders/skinned_model.vert"),
                                fs.getAbsPath("Shaders/skinned_model.frag"))) return -1;

    skinnedModelShader.use();

    SkinnedModel sk(
            fs.getAbsPath("res/mesh/dwarf/dwarf.x"),
  // "res/mesh/guard/boblampclean.md5mesh",
            skinnedModelShader,
            fs.getAbsPath("res/mesh/dwarf/"),
            true 
           );


    if(!sk.init()) return -1;

    Timer& timer = engine.getTimer();
    Camera& camera = engine.getCamera();
    glm::mat4& proj = camera.getProj();
    glm::mat4& view = camera.getView();
    
   // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

  
    while(engine.windowIsOpen())
    {  
        float dt = timer.tick();
    
        glm::mat4 viewProj = proj * view;
        
        engine.pollEvents();
        engine.handleCameraMovement(dt);
 
        glClearColor(0.0f,0.0f,0.3f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        sk.update(timer.getCurrentTime());
        sk.draw(viewProj, skinnedModelShader);

        engine.swapBuffers();
    }

    return 0;
}





