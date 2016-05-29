#include <iostream>

#include "Engine.h"
#include "FileSystem.h"
#include "Shader.h"
#include "Quad.h"
#include "Model.h" 
#include "SkinnedModel.h"
#include "SkinnedModelTechnique.h"

int main()
{
    Engine engine;

    if(!engine.init()) return -1;

    glEnable(GL_DEPTH_TEST);

    Technique qTech("Shaders/quad.vert", "Shaders/quad.frag");
    qTech.setHandleSampler().setHandleWorldViewProj();
    Quad q;   
    q.init(FileSystem::getInstance().getAbsPath("res/img.jpg").c_str());
    SkinnedModel sk("res/mesh/dwarf/dwarf.x",
                    "res/mesh/dwarf/",
                    ModelFlag_STRIP_TEXTURE_PATH | ModelFlag_USE_ABS_PATH
           );

    if(!sk.init()) return -1;

    SkinnedModelTechnique skTech("Shaders/skinned_model.vert",
                                 "Shaders/skinned_model.frag");

    skTech.setHandleBoneTransforms(sk.mNumBones)
          .setHandleMaterials(sk.mMaterials)
          .setHandleWorldViewProj();

    
    Model model("res/mesh/guard/boblampclean.md5mesh",
                "res/mesh/guard/",
                ModelFlag_USE_ABS_PATH);

    if(!model.init()) return -1;

    ModelTechnique mTech("Shaders/model.vert",
                         "Shaders/model.frag");

    mTech.setHandleMaterials(model.mMaterials)
         .setHandleWorldViewProj();
     
    Model model2("res/mesh/nanosuit/nanosuit.obj", 
                 "res/mesh/nanosuit/",
                 ModelFlag_USE_ABS_PATH);

    if(!model2.init()) return -1;

    ModelTechnique m2Tech("Shaders/model.vert", 
                          "Shaders/model.frag");
    m2Tech.setHandleMaterials(model2.mMaterials)
         .setHandleWorldViewProj();

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

        mTech.use();
        mTech.setUniformWorldViewProj(viewProj);
        model.draw(mTech);
  
        m2Tech.use();
        m2Tech.setUniformWorldViewProj(viewProj);
        model2.draw(m2Tech);
       
        skTech.use();
        skTech.setUniformWorldViewProj(viewProj);
        sk.update(timer.getCurrentTime());
        sk.draw(skTech);

        qTech.use();
        qTech.setUniformWorldViewProj(viewProj);
        q.draw(qTech);

        engine.swapBuffers();
    }

    return 0;
}





