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
#include "g_buffer.h"
#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/string_cast.hpp"
#include "sphere.h"

int main()
{
    srand (static_cast <unsigned> (time(0)));

    Engine engine;    
    if(!engine.init()) return -1;

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f,0.0f,0.0f,0.0f);

    Window& window = engine.getWindow(); 
    GBuffer gBuffer;
    if(!gBuffer.init(window.getWidth(), window.getHeight())) return false;

    size_t nl = 8;
    float min = 0.0f, max = 1.0f;
    float minPos = 1.0f, maxPos = 10.0f;
    float minRad = 5.0f, maxRad = 175.0f;
    
    DirectionalLight dl;
    dl.direction = Utils::getRandVec3(minPos,maxPos);
 //   dl.color = Utils::getRandVec3(min,max);
    glm::normalize(dl.direction);

    std::vector<SpotLight> sl;
    std::vector<PointLight> pl;
    std::vector<glm::mat4> plScales;
    std::vector<glm::mat4> plWVP;
    //plWorlds.resize(nl);
    for(size_t i=0;i<nl;++i)
    {

        SpotLight spot;
        spot.position = Utils::getRandVec3(minPos, maxPos);
        spot.direction = Utils::getRandVec3(minPos, maxPos);
        glm::normalize(spot.direction);
        spot.color = Utils::getRandVec3(min, max);

        

        PointLight point(Utils::randF(minRad, maxRad));

        point.position = Utils::getRandVec3(minPos, maxPos);


        float scale = point.getRadius();
        LOG(scale);
        glm::mat4 scaleMtx = glm::mat4(1.0f);
        scaleMtx *= scale;
        plScales.push_back(scaleMtx);
        plWVP.push_back(scaleMtx);
        sl.push_back(spot);
        pl.push_back(point);
    }
    exit(1);

    Technique stencilTech("shaders/base.vert", "shaders/base.frag");
    stencilTech.setHandleWorldViewProj().setHandlePointLight();

    Technique sTech("shaders/base.vert", "shaders/deferred_light.frag");
    sTech.setHandleWorldViewProj().setHandlePointLight().setHandleViewPos();
    Sphere s;
    s.init(25, 25, 1.0f);

    Technique qTech("shaders/quad.vert", "shaders/deferred_geometry2.frag");
    qTech.setHandleWorldViewProj().setHandleDirectionalLight().setHandleViewPos();
    Quad q;   
    q.initBuffers();

    SkinnedModel sk("res/mesh/dwarf/dwarf.x",
                    "res/mesh/dwarf/",
                    ModelFlag_STRIP_TEXTURE_PATH | ModelFlag_USE_ABS_PATH);

    if(!sk.init()) return -1;

    SkinnedModelTechnique skTech("shaders/skinned_model.vert",
                                 "shaders/deferred_geometry.frag");

    skTech.setHandleBoneTransforms(sk.mNumBones)
          .setHandleMaterials(sk.mMaterials)
          .setHandleWorldViewProj()
         .setHandleWorld();
    
    Model model("res/mesh/guard/boblampclean.md5mesh",
                "res/mesh/guard/",
                ModelFlag_USE_ABS_PATH);

    if(!model.init()) return -1;

    ModelTechnique mTech("shaders/model.vert",
                         "shaders/deferred_geometry.frag");

    mTech.setHandleMaterials(model.mMaterials)
         .setHandleWorldViewProj()
         .setHandleWorld();
     
    Model model2("res/mesh/nanosuit/nanosuit.obj", 
                 "res/mesh/nanosuit/",
                 ModelFlag_USE_ABS_PATH);

    if(!model2.init()) return -1;

    ModelTechnique m2Tech("shaders/model.vert", 
                          "shaders/deferred_geometry.frag");
    m2Tech.setHandleMaterials(model2.mMaterials)
         .setHandleWorldViewProj()
         .setHandleWorld()
         .setHandleViewPos();

    Timer& timer = engine.getTimer();
    Camera& camera = engine.getCamera();
    glm::mat4& proj = camera.getProj();
    glm::mat4& view = camera.getView();
    
    glm::mat4 world = glm::mat4(1.0f);

    qTech.use();
    qTech.setUniformWorldViewProj(world)
        .setUniformDirectionalLight(dl);
    
    skTech.use();
    skTech.setUniformWorld(world);

    mTech.use();
    mTech.setUniformWorld(world);

    m2Tech.use();
    m2Tech.setUniformWorld(world);    
    
   
   // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    float t = 0.0f; 

    while(engine.windowIsOpen())
    {  
        float dt = timer.tick();
        t += dt;


        glm::mat4 viewProj = proj * view;
        glm::vec3& eyePos = camera.getEye();

        engine.pollEvents();
        engine.handleCameraMovement(dt);
        
        if(t > 0.03f) 
        {
            for(size_t i=0;i<nl;++i)
            {
                sl[i].position =  glm::rotate(sl[i].position, dt, glm::vec3(0.0f,1.0f,0.0f));
                pl[i].position = glm::rotate(pl[i].position, dt, glm::vec3(0.0f,1.0f,0.0f));
                glm::mat4 translate = glm::translate(pl[i].position);

                plWVP[i] = viewProj *  plScales[i] * translate;

            }
          /*  mTech.use();
            mTech.setUniformPointLights(pl).setUniformSpotLights(sl);
            m2Tech.use();
            m2Tech.setUniformPointLights(pl).setUniformSpotLights(sl);
            skTech.use();
            skTech.setUniformPointLights(pl).setUniformSpotLights(sl);*/
            t = 0.0f;
        }

      //  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

      //  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer.mBuffer);

     /*   glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.mBuffer);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glBlitFramebuffer(0,0,window.getWidth(),window.getHeight(),
                          0,0,window.getWidth(),window.getHeight(),
                          GL_COLOR_BUFFER_BIT, GL_LINEAR);

    */
        gBuffer.startGeometryPass1();



        mTech.use();
        mTech.setUniformWorldViewProj(viewProj)
             .setUniformViewPos(eyePos);
        model.draw(mTech);
  
        m2Tech.use();
        m2Tech.setUniformWorldViewProj(viewProj)
              .setUniformViewPos(eyePos);
        model2.draw(m2Tech);
       
        skTech.use();
        skTech.setUniformWorldViewProj(viewProj)
              .setUniformViewPos(eyePos);
        sk.update(timer.getCurrentTime());
        sk.draw(skTech);

        sTech.use();
        sTech.setUniformWorldViewProj(viewProj);



        gBuffer.endGeometryPass1();


        stencilTech.use();
        gBuffer.startStencilPass();

        for(size_t i=0;i<pl.size();++i)
        {
            PointLight& p = pl[i];
            stencilTech
                .setUniformWorldViewProj(plWVP[i])
                .setUniformPointLight(p);
            s.draw();
        }

        gBuffer.endStencilPass();

        sTech.use();
        sTech.setUniformViewPos(eyePos);

        gBuffer.startLightPass();
        
        for(size_t i=0;i<pl.size();++i)
        {
            PointLight& p = pl[i];
            sTech
                .setUniformWorldViewProj(plWVP[i])
                .setUniformPointLight(p);
            s.draw();
        }

        gBuffer.endLightPass();


        qTech.use();
        qTech.setUniformViewPos(eyePos);
        gBuffer.startGeometryPass2();
        
        q.draw();

        gBuffer.endGeometryPass2(window.getWidth(), window.getHeight());
        
        engine.swapBuffers();
    }

    return 0;
}





