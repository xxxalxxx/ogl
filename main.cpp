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
#include "ssao.h"

int main()
{
    srand (static_cast <unsigned> (time(0)));

    Engine engine;    
    if(!engine.init()) return -1;

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f,0.0f,0.0f,0.0f);

    Window& window = engine.getWindow(); 
    GBuffer gBuffer;
    if(!gBuffer.init(window.getWidth(), window.getHeight())) return -1;
    SSAO ssao;
    if(!ssao.init(window.getWidth(), window.getHeight())) return -1;


    size_t nl = 15;
    float min = 0.0f, max = 1.0f;
    float minPos = 1.0f, maxPos = 25.0f;
    float minRad = 50.0f, maxRad = 150.0f;
    
    DirectionalLight dl;
    dl.direction = Utils::getRandVec3(minPos,maxPos);
 //   dl.color = Utils::getRandVec3(min,max);
    glm::normalize(dl.direction);

    std::vector<SpotLight> sl;
    std::vector<PointLight> pl;
    std::vector<glm::mat4> plScales;
    std::vector<glm::mat4> plWorlds;
    std::vector<glm::mat4> plWVP;

    for(size_t i=0;i<nl;++i)
    {

        SpotLight spot;
        spot.position = Utils::getRandVec3(minPos, maxPos);
        spot.direction = Utils::getRandVec3(minPos, maxPos);
        glm::normalize(spot.direction);
        spot.color = Utils::getRandVec3(min, max);
        

        PointLight point(Utils::randF(minRad, maxRad),Utils::getRandVec3(min,max) );
        point.position = Utils::getRandVec3(minPos, maxPos);


        float scale = point.getRadius();
        LOG(scale);

        plScales.push_back(glm::mat4());
        plWorlds.push_back(glm::mat4(1.0f));

        glm::mat4& scaleMtx = plScales.back();
        glm::vec3 sv = glm::vec3(scale,scale,scale);
        scaleMtx = glm::scale(scaleMtx, sv);

        plWVP.push_back(scaleMtx);
        sl.push_back(spot);
        pl.push_back(point);
    }
    Technique ssaoTech("shaders/ssao.vert", "shaders/ssao.frag");
    ssaoTech.setHandleProj().setHandleWorldViewProj();
    std::vector<GLuint> ssaoKernelHandles = ssao.getUniformSampleHandles(ssaoTech.mUniforms.program);
    GLuint ssaoNoiseHandle = glGetUniformLocation(ssaoTech.mUniforms.program, "u_Noise");
    GLuint ssaoDepthHandle = glGetUniformLocation(ssaoTech.mUniforms.program, "u_Depth");
    GLuint ssaoNormalHandle = glGetUniformLocation(ssaoTech.mUniforms.program, "u_Normal");
    GLuint ssaoRadiusHandle = glGetUniformLocation(ssaoTech.mUniforms.program, "u_Radius");

    Technique ssaoBlurTech("shaders/quad.vert", "shaders/ssao_blur.frag");
    ssaoBlurTech.setHandleWorldViewProj();
    GLuint ssaoBlurSSAOHandle = glGetUniformLocation(ssaoBlurTech.mUniforms.program, "u_SSAO");
    
    Technique stencilTech("shaders/ssao.vert", "shaders/base.frag");
    stencilTech.setHandleWorldViewProj().setHandlePointLight();

    Technique sTech("shaders/deferred_light.vert", "shaders/deferred_light.frag");
    sTech
        .setHandleWorldViewProj()
        .setHandlePointLight()
        .setHandleViewPos()
        .setHandleView()
        .setHandleProj()
        .setHandleWorld();

    GLuint sTechCamWorldHandle = glGetUniformLocation(sTech.mUniforms.program, "u_CamWorld"); 
    GLuint sTechDepthHandle = glGetUniformLocation(sTech.mUniforms.program, "u_Depth");
    GLuint sTechColorHandle = glGetUniformLocation(sTech.mUniforms.program, "u_Color");
    GLuint sTechNormalHandle = glGetUniformLocation(sTech.mUniforms.program, "u_Normal");

    Sphere s;
    s.init(25, 25, 1.0f);

    Technique q2Tech("shaders/quad.vert", "shaders/deferred_geometry3.frag");
    q2Tech.setHandleWorldViewProj().setHandleDirectionalLight().setHandleViewPos();

    Technique qTech("shaders/quad.vert", "shaders/deferred_geometry2.frag");
    qTech
        .setHandleWorldViewProj()
        .setHandleDirectionalLight()
        .setHandleViewPos();

    GLuint qTechDepthHandle = glGetUniformLocation(qTech.mUniforms.program, "u_Depth");
    GLuint qTechColorHandle = glGetUniformLocation(qTech.mUniforms.program, "u_Color");
    GLuint qTechNormalHandle = glGetUniformLocation(qTech.mUniforms.program, "u_Normal");
    GLuint qTechResultHandle = glGetUniformLocation(qTech.mUniforms.program, "u_Result");
    GLuint qTechSSAOHandle = glGetUniformLocation(qTech.mUniforms.program, "u_SSAO");




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
  
    q2Tech.use();
    q2Tech.setUniformWorldViewProj(world)
        .setUniformDirectionalLight(dl);

    qTech.use();
    qTech.setUniformWorldViewProj(world)
        .setUniformDirectionalLight(dl);
    
    skTech.use();
    skTech.setUniformWorld(world);

    mTech.use();
    mTech.setUniformWorld(world);

    m2Tech.use();
    m2Tech.setUniformWorld(world);

    ssaoTech.use();
    ssaoTech.setUniformWorldViewProj(world);
    glUniform1f(ssaoRadiusHandle, 1.0f);
    ssao.setUniformSampleHandles(ssaoKernelHandles);

    ssaoBlurTech.use();    
    ssaoBlurTech.setUniformWorldViewProj(world);

    LOG(window.getWidth() << " " << window.getHeight());

   // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    float t = 0.0f; 

    while(engine.windowIsOpen())
    {  
        float dt = timer.tick();
        t += dt;


        glm::mat4 viewProj = proj * view;
        glm::mat3 camWorld = glm::transpose(glm::mat3(view));
        glm::vec3& eyePos = camera.getEye();

        engine.pollEvents();
        engine.handleCameraMovement(dt);
        
        if(t > 0.03f) 
        {
            for(size_t i=0;i<nl;++i)
            {
                sl[i].position =  glm::rotate(sl[i].position, dt, glm::vec3(0.0f,1.0f,0.0f));
                pl[i].position = glm::rotate(pl[i].position,Utils::randF(min,max) *  dt, glm::vec3(0.0f,1.0f,0.0f));
                glm::mat4 translate = glm::translate(pl[i].position);
                plWorlds[i] = translate * plScales[i];
                plWVP[i] = viewProj *  translate;
            }

            t = 0.0f;
        }

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

        /*
         * geometry 1
         *
         */

        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.mBuffer);

        GLuint attachments[] = { GL_COLOR_ATTACHMENT0, 
                                 GL_COLOR_ATTACHMENT1,
                                 GL_COLOR_ATTACHMENT2 };

        glDrawBuffers(3, attachments);
        
        glDepthMask(GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);


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

        /*
         * SSAO pass
         *
         */

        glBindFramebuffer(GL_FRAMEBUFFER, ssao.mBuffer);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        ssaoTech.use();
        ssaoTech.setUniformProj(proj);

        GLuint attachmentsSSAO[] = { GL_COLOR_ATTACHMENT0 };

        glDrawBuffers(1, attachmentsSSAO);


        glActiveTexture(GL_TEXTURE0);
        glUniform1i(ssaoNormalHandle, 0); 
        glBindTexture(GL_TEXTURE_2D, gBuffer.mNormal);
         
        glActiveTexture(GL_TEXTURE1);
        glUniform1i(ssaoDepthHandle, 1); 
        glBindTexture(GL_TEXTURE_2D, gBuffer.mDepth); 
  
        glActiveTexture(GL_TEXTURE2);
        glUniform1i(ssaoNoiseHandle, 2); 
        glBindTexture(GL_TEXTURE_2D, ssao.mNoise); 


        q.draw();


        ssaoBlurTech.use();


        GLuint attachmentsSSAOBlur[] = { GL_COLOR_ATTACHMENT1 };

        glDrawBuffers(1, attachmentsSSAOBlur);


        glActiveTexture(GL_TEXTURE0);
        glUniform1i(ssaoBlurSSAOHandle, 0); 
        glBindTexture(GL_TEXTURE_2D, ssao.mSSAO);


        q.draw();


        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.mBuffer);

        /*
         * stencil pass
         *
         */

        stencilTech.use();

        glDepthMask(GL_FALSE); //dont upd depth
    
        glEnable(GL_STENCIL_TEST); 
        glDrawBuffer(GL_NONE); //dont draw lights
        glEnable(GL_DEPTH_TEST); //depth test for incrementing
        glDisable(GL_CULL_FACE); //dont cull back or front faces for incrementing
        
        glClear(GL_STENCIL_BUFFER_BIT);
        
        glStencilFunc(GL_ALWAYS, 0, 0);
        
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

        
        for(size_t i=0;i<pl.size();++i)
        {
            glm::mat4 mvp = plWVP[i] * plScales[i];
            stencilTech.setUniformWorldViewProj(mvp);
            s.draw();
        }

        /*
         * light pass
         *
         */

        sTech.use();
        sTech.setUniformViewPos(eyePos);
        sTech.setUniformProj(proj);
        sTech.setUniformView(view);
        glUniformMatrix3fv(sTechCamWorldHandle, 1, GL_FALSE, glm::value_ptr(camWorld));

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(sTechNormalHandle, 0); 
        glBindTexture(GL_TEXTURE_2D, gBuffer.mNormal);
         
        glActiveTexture(GL_TEXTURE1);
        glUniform1i(sTechColorHandle, 1); 
        glBindTexture(GL_TEXTURE_2D, gBuffer.mColor); 
  
        glActiveTexture(GL_TEXTURE2);
        glUniform1i(sTechDepthHandle, 2); 
        glBindTexture(GL_TEXTURE_2D, gBuffer.mDepth); 

        GLuint attachments2[] = { GL_COLOR_ATTACHMENT2 };

        glDrawBuffers(1, attachments2);

        glStencilFunc(GL_NOTEQUAL, 0, 0xff);
        glDisable(GL_DEPTH_TEST);

        //blend all lights drawn
        glEnable(GL_BLEND); 
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);
        
        //when camera inside volume, back faces must not be culled
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        
        for(size_t i=0;i<pl.size();++i)
        {
            glm::mat4 mvp = plWVP[i] * plScales[i];
            sTech.setUniformWorldViewProj(mvp);     
            sTech.setUniformWorld(plWorlds[i]);
            sTech.setUniformPointLight(pl[i]);
            s.draw();
        }


     
        /*
         * geometry 2
         *
         */
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        qTech.use();
        qTech.setUniformViewPos(eyePos);
        
        glCullFace(GL_BACK);
        glDisable(GL_BLEND);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_DEPTH_TEST);
       
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(qTechNormalHandle, 0); 
        glBindTexture(GL_TEXTURE_2D, gBuffer.mNormal);

        glActiveTexture(GL_TEXTURE1);
        glUniform1i(qTechColorHandle, 1); 
        glBindTexture(GL_TEXTURE_2D, gBuffer.mColor);

        glActiveTexture(GL_TEXTURE2);
        glUniform1i(qTechResultHandle, 2); 
        glBindTexture(GL_TEXTURE_2D, gBuffer.mResult);

        glActiveTexture(GL_TEXTURE3);
        glUniform1i(qTechDepthHandle, 3); 
        glBindTexture(GL_TEXTURE_2D, gBuffer.mDepth);
        
        glActiveTexture(GL_TEXTURE4);
        glUniform1i(qTechSSAOHandle, 4); 
        glBindTexture(GL_TEXTURE_2D, ssao.mBlurredSSAO);
     
        q.draw();

        /*
         * end deferred
         *
         */

        glDepthMask(GL_TRUE);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.mBuffer);

        glBlitFramebuffer(0, 0, window.getWidth(), window.getHeight(),
                          0, 0, window.getWidth(), window.getHeight(),
                         GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);

        //back faces still culled
        
        /*
         * after deferred
         *
         */

        q2Tech.use();
        for(size_t i=0;i<nl;++i)
        {
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(glGetUniformLocation(q2Tech.mUniforms.program, "u_Depth"), 0); 
            glBindTexture(GL_TEXTURE_2D, gBuffer.mDepth);

            q2Tech.setUniformWorldViewProj(plWVP[i]);
            q.draw();
        }
        engine.swapBuffers();
    }

    return 0;
}


