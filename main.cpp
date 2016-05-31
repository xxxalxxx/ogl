#include <iostream>

#include "engine.h"
#include "file_system.h"
#include "shader.h"
#include "quad.h"
#include "model.h" 
#include "skinned_model.h"
#include "skinned_model_technique.h"
#include "utils.h"
#include "light.h"
#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/string_cast.hpp"

int main()
{
    srand (static_cast <unsigned> (time(0)));

    Engine engine;    
    if(!engine.init()) return -1;

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f,0.0f,0.3f,1.0f);

    size_t nl = 8;
    float min = 0.0f, max = 1.0f;
    float minPos = 1.0f, maxPos = 10.0f;
    float minRad = 100.0f, maxRad = 255.0f;
    
    DirectionalLight dl;
    dl.direction = Utils::getRandVec3(minPos,maxPos);
    glm::normalize(dl.direction);

   // dl.ambient = Utils::getRandVec3(min, max);
   // dl.specular = Utils::getRandVec3(min, max);
   // dl.diffuse = Utils::getRandVec3(min , max);
    

    std::vector<SpotLight> sl;
    std::vector<PointLight> pl;
    for(size_t i=0;i<nl;++i)
    {
        SpotLight spot;
        spot.position = Utils::getRandVec3(minPos, maxPos);
        spot.direction = Utils::getRandVec3(minPos, maxPos);
        glm::normalize(spot.direction);

     //   spot.ambient = Utils::getRandVec3(min, max);
      //  spot.specular = Utils::getRandVec3(min, max);
      //  spot.diffuse = Utils::getRandVec3(min , max);

        PointLight point(Utils::randF(minRad, maxRad));
        LOG(point.a0 << " " << point.a1 << " " << point.a2);
        point.position = Utils::getRandVec3(minPos, maxPos);
        
        point.ambient = Utils::getRandVec3(min, max);
        point.specular = Utils::getRandVec3(min, max);
        point.diffuse = Utils::getRandVec3(min , max);



        sl.push_back(spot);
        pl.push_back(point);
    }



    Technique qTech("shaders/quad.vert", "shaders/quad.frag");
    qTech.setHandleSampler().setHandleWorldViewProj();
    Quad q;   
    q.init(FileSystem::getInstance().getAbsPath("res/img.jpg").c_str());
    SkinnedModel sk("res/mesh/dwarf/dwarf.x",
                    "res/mesh/dwarf/",
                    ModelFlag_STRIP_TEXTURE_PATH | ModelFlag_USE_ABS_PATH);

    if(!sk.init()) return -1;

    SkinnedModelTechnique skTech("shaders/skinned_model.vert",
                                 "shaders/skinned_model.frag");

    skTech.setHandleBoneTransforms(sk.mNumBones)
          .setHandleMaterials(sk.mMaterials)
          .setHandleWorldViewProj();

    
    Model model("res/mesh/guard/boblampclean.md5mesh",
                "res/mesh/guard/",
                ModelFlag_USE_ABS_PATH);

    if(!model.init()) return -1;

    ModelTechnique mTech("shaders/model.vert",
                         "shaders/model.frag");

    mTech.setHandleMaterials(model.mMaterials)
         .setHandleWorldViewProj()
         .setHandleViewPos()
         .setHandleLights(true, nl, nl);
     
    Model model2("res/mesh/nanosuit/nanosuit.obj", 
                 "res/mesh/nanosuit/",
                 ModelFlag_USE_ABS_PATH);

    if(!model2.init()) return -1;

    ModelTechnique m2Tech("shaders/model.vert", 
                          "shaders/model.frag");
    m2Tech.setHandleMaterials(model2.mMaterials)
         .setHandleWorldViewProj()
         .setHandleViewPos()
         .setHandleLights(true, nl, nl);

    Timer& timer = engine.getTimer();
    Camera& camera = engine.getCamera();
    glm::mat4& proj = camera.getProj();
    glm::mat4& view = camera.getView();

    mTech.use();
    mTech.setUniformLights(&dl, &sl, &pl);
    m2Tech.use();
    m2Tech.setUniformLights(&dl, &sl, &pl);    
   // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    float t = 0.0f; 

    while(engine.windowIsOpen())
    {  
        float dt = timer.tick();
        t += dt;

        if(t > 0.03f) 
        {
            for(size_t i=0;i<nl;++i)
            {
              //  sl[i].position =  glm::rotate(sl[i].position, dt, glm::vec3(0.0f,1.0f,0.0f));
                LOG(pl[i].a2);
                pl[i].position = glm::rotate(pl[i].position, dt, glm::vec3(0.0f,1.0f,0.0f));
            }
            mTech.use();
            mTech.setUniformPointLights(pl).setUniformSpotLights(sl);
            m2Tech.use();
            m2Tech.setUniformPointLights(pl).setUniformSpotLights(sl);
            t = 0.0f;
            LOG("in f");
        }

        glm::mat4 viewProj = proj * view;
       
        engine.pollEvents();
        engine.handleCameraMovement(dt);
 
      //  glClearColor(0.1f,0.1f,0.1f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        if(timer.getCurrentTime())

        mTech.use();
        mTech.setUniformWorldViewProj(viewProj)
             .setUniformViewPos(camera.getEye());
        model.draw(mTech);
  
        m2Tech.use();
        m2Tech.setUniformWorldViewProj(viewProj)
              .setUniformViewPos(camera.getEye());
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





