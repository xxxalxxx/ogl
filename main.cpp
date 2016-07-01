#include <iostream>
#include <math.h>

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
#include "cone.h"
#include "skybox.h"
#include "heightmap.h"

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
    if(!ssao.init(window.getWidth(), window.getHeight(), 1.0f, 1.0f)) return -1;
    gBuffer.setSSAO(ssao);

 

    size_t nl = 15;
    float min = 0.0f, max = 1.0f;
    float minPos = 1.0f, maxPos = 25.0f;
    float minRad = 120.0f, maxRad = 150.0f;
    float minAngle = 0.9f * M_PI_4, maxAngle = M_PI_4;
    glm::vec3 down(0.0f, -1.0f, 0.0f); 
    glm::vec3 up(0.0f, 1.0f, 0.0f);    

    DirectionalLight dl;
    dl.direction = Utils::getRandVec3(minPos,maxPos);
 //   dl.color = Utils::getRandVec3(min,max);
    glm::normalize(dl.direction);

    std::vector<SpotLight> sl;
    std::vector<PointLight> pl;
    
    std::vector<glm::mat4> slScales;
    std::vector<glm::mat4> slWorlds;
    std::vector<glm::mat4> slWVP;
    std::vector<glm::mat4> slRotations;

    std::vector<glm::mat4> plScales;
    std::vector<glm::mat4> plWorlds;
    std::vector<glm::mat4> plWVP;

    for(size_t i=0;i<nl;++i)
    {
        float angle = Utils::randF(minAngle, maxAngle);
        SpotLight spot(
                    Utils::randF(minRad, maxRad),
                    Utils::getRandVec3(min, max),
                    angle, 0.1f * angle
                );

        spot.position = Utils::getRandVec3(minPos, maxPos);
        spot.direction = glm::normalize(Utils::getRandVec3(-1.0f, 1.0f));
        float spotScaleY = spot.getRadius();
        float spotScaleXZ = spotScaleY * atan(0.5f * spot.cutoff);
        glm::vec3 axis = glm::cross(down, spot.direction);
        slScales.push_back(glm::mat4());
        slRotations.push_back(glm::mat4());
        slWorlds.push_back(glm::mat4(1.0f));
        slWVP.push_back(glm::mat4(1.0f));
        
        glm::mat4& scaleSlMtx = slScales.back();
        glm::vec3 scaleSlVec = glm::vec3(spotScaleXZ,spotScaleY,spotScaleXZ);
        scaleSlMtx = glm::scale(scaleSlMtx, scaleSlVec);
        LOG(glm::to_string(scaleSlMtx));
        glm::mat4& rotMtx = slRotations.back();
        glm::vec3 bi = glm::cross(axis, spot.direction);
        glm::mat3 rotmat = glm::mat3(axis, -spot.direction, bi);

        rotMtx = glm::mat4(rotmat);
        
        PointLight point(Utils::randF(minRad, maxRad), Utils::getRandVec3(min,max) );
        point.position = Utils::getRandVec3(minPos, maxPos);

        float scale = point.getRadius();
        
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
    ssaoTech
        .setHandleProj()
        .setHandleWorldViewProj()
        .setHandleView();

    std::vector<GLuint> ssaoKernelHandles = ssao.getUniformSampleHandles(ssaoTech.mUniforms.program);
    GLuint ssaoNoiseHandle = glGetUniformLocation(ssaoTech.mUniforms.program, "u_Noise");
    GLuint ssaoDepthHandle = glGetUniformLocation(ssaoTech.mUniforms.program, "u_Depth");
    GLuint ssaoNormalHandle = glGetUniformLocation(ssaoTech.mUniforms.program, "u_Normal");
    GLuint ssaoRadiusHandle = glGetUniformLocation(ssaoTech.mUniforms.program, "u_Radius");
    GLuint ssaoScaleSSAOHandle = glGetUniformLocation(ssaoTech.mUniforms.program, "u_ScaleSSAO");
    
    Technique ssaoBlurTech("shaders/ssao_blur.vert", "shaders/ssao_blur.frag");
    ssaoBlurTech.setHandleWorldViewProj();
    GLuint ssaoBlurSSAOHandle = glGetUniformLocation(ssaoBlurTech.mUniforms.program, "u_SSAO");
    GLuint ssaoBlurScaleSSAOHandle = glGetUniformLocation(ssaoBlurTech.mUniforms.program, "u_ScaleSSAO");

    Technique stencilTech("shaders/base.vert", "shaders/base.frag");
    stencilTech.setHandleWorldViewProj().setHandlePointLight();

    Technique sTech("shaders/deferred_point_light.vert", "shaders/deferred_point_light.frag");
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
    if(!s.init(25, 25, 1.0f)) return -1;
    
    Cone cone;
    if(!cone.init(25)) return -1;

    Technique spotLightTech("shaders/deferred_spot_light.vert", "shaders/deferred_spot_light.frag");
    spotLightTech
        .setHandleWorldViewProj()
        .setHandleSpotLight()
        .setHandleViewPos()
        .setHandleView()
        .setHandleProj()
        .setHandleWorld();

    GLuint spotLightTechCamWorldHandle = glGetUniformLocation(spotLightTech.mUniforms.program, "u_CamWorld"); 
    GLuint spotLightTechDepthHandle = glGetUniformLocation(spotLightTech.mUniforms.program, "u_Depth");
    GLuint spotLightTechColorHandle = glGetUniformLocation(spotLightTech.mUniforms.program, "u_Color");
    GLuint spotLightTechNormalHandle = glGetUniformLocation(spotLightTech.mUniforms.program, "u_Normal");

    Technique q2Tech("shaders/quad.vert", "shaders/deferred_geometry3.frag");
    q2Tech
        .setHandleWorldViewProj()
        .setHandleDirectionalLight()
        .setHandleViewPos();

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

    skTech
        .setHandleBoneTransforms(sk.mNumBones)
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
    m2Tech
        .setHandleMaterials(model2.mMaterials)
        .setHandleWorldViewProj()
        .setHandleWorld()
        .setHandleViewPos();

    Timer& timer = engine.getTimer();
    Camera& camera = engine.getCamera();
    glm::mat4& proj = camera.getProj();
    glm::mat4& view = camera.getView();
    glm::mat4 world = glm::mat4(1.0f);
  
    Skybox skybox;
    skybox.init();

    Technique cubeMapTech("shaders/cubemap.vert",
                         "shaders/cubemap.frag");

    FileSystem& fs = engine.getFileSystem();
    TextureManager& textureManager = TextureManager::getInstance();
    GLuint cubeMapWVPHandle = glGetUniformLocation(cubeMapTech.mUniforms.program, "u_WVP");
    GLuint cubeMapSamplerHandle = glGetUniformLocation(cubeMapTech.mUniforms.program, "u_Sampler");
    GLuint cubeMapTex = textureManager.loadCubeMap(
                fs.getAbsPath("res/cubemap/mp_plains/plains-of-abraham_lf.tga"),
                fs.getAbsPath("res/cubemap/mp_plains/plains-of-abraham_rt.tga"),
                fs.getAbsPath("res/cubemap/mp_plains/plains-of-abraham_up.tga"),
                fs.getAbsPath("res/cubemap/mp_plains/plains-of-abraham_dn.tga"),
                fs.getAbsPath("res/cubemap/mp_plains/plains-of-abraham_ft.tga"),
                fs.getAbsPath("res/cubemap/mp_plains/plains-of-abraham_bk.tga")
    );

    q2Tech.use();
    q2Tech
        .setUniformWorldViewProj(world)
        .setUniformDirectionalLight(dl);

    qTech.use();
    qTech
        .setUniformWorldViewProj(world)
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
    glUniform1f(ssaoScaleSSAOHandle, 1.0f/ssao.mScaleSSAO);

    ssao.setUniformSampleHandles(ssaoKernelHandles);

    ssaoBlurTech.use();    
    ssaoBlurTech.setUniformWorldViewProj(world);
    glUniform1f(ssaoBlurScaleSSAOHandle, ssao.mScaleSSAO/ssao.mScaleBlurSSAO/ssao.mScaleSSAO);

   // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
   

    HeightMap hm;
    std::vector<HeightMap::Layer> layers;
    HeightMap::Layer l1("res/heightmap/hm1/sand.jpg", 0.2f);
    HeightMap::Layer l2("res/heightmap/hm1/sand_grass.jpg", 0.5f);
    HeightMap::Layer l3("res/heightmap/hm1/rocks.jpg", 1.0f);

    layers.push_back(l1);
    layers.push_back(l2);
    layers.push_back(l3);

    glm::mat4 heightMapScale;
    heightMapScale = glm::scale(heightMapScale, glm::vec3(50.0f, 10.0f, 50.0f));


    Technique heightMapTech("shaders/heightmap.vert", "shaders/heightmap.frag");
    heightMapTech        
        .setHandleWorldViewProj()
        .setHandleSampler();

    hm.init("res/heightmap/hm1/hm.jpg",
            layers,
            NULL,
            HeightMapFlag_GEN_NORMALS | HeightMapFlag_USE_ABS_PATH     
        );

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
                pl[i].position = glm::rotate(pl[i].position, 0.3f * Utils::randF(min,max) *  dt, glm::vec3(0.0f,1.0f,0.0f));
                glm::mat4 translate = glm::translate(pl[i].position);
                plWorlds[i] = translate * plScales[i];
                plWVP[i] = viewProj *  translate;

                sl[i].position =  glm::rotate(sl[i].position, 1.5f *  Utils::randF(min,max) * dt, glm::vec3(0.0f,1.0f,0.0f));
                glm::mat4 w = glm::translate(sl[i].position);
                glm::mat4 r = glm::mat4_cast(glm::angleAxis( 0.5f * Utils::randF(min,max) *  dt, glm::vec3(0.0f,1.0f,0.0f)));
                slRotations[i] =  r * slRotations[i];
                sl[i].direction = glm::vec3( r * glm::vec4(sl[i].direction, 0.0f) ); 
                slWorlds[i] = w * slRotations[i] * slScales[i];
                slWVP[i] = viewProj * slWorlds[i];
            }

            t = 0.0f;
        }

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

        /*
         * geometry 1
         *
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

        /*
         * SSAO pass
         *
         */
        ssaoTech.use();
        ssaoTech.setUniformProj(proj);

        ssao.startSSAOPass();

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

        ssao.startSSAOBlurPass();

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(ssaoBlurSSAOHandle, 0); 
        glBindTexture(GL_TEXTURE_2D, ssao.mSSAO);


        q.draw();
 
        /*
         * stencil pass
         *
         */

        stencilTech.use();

        gBuffer.startStencilPass();
        
        for(size_t i=0;i<pl.size();++i)
        {
            glm::mat4 wvp = plWVP[i] * plScales[i];
            stencilTech.setUniformWorldViewProj(wvp);
            s.draw();
        }

        for(size_t i=0;i<sl.size();++i)
        {
            stencilTech.setUniformWorldViewProj(slWVP[i]);            
         //   cone.draw(); 
        }

        /*
         * light pass
         *
         */
        
        gBuffer.startLightPass();

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

      
        for(size_t i=0;i<pl.size();++i)
        {
            glm::mat4 mvp = plWVP[i] * plScales[i];
            sTech.setUniformWorldViewProj(mvp);     
            sTech.setUniformWorld(plWorlds[i]);
            sTech.setUniformPointLight(pl[i]);
            s.draw();
        }


        spotLightTech.use();
        
        spotLightTech.setUniformViewPos(eyePos);
        spotLightTech.setUniformProj(proj);
        spotLightTech.setUniformView(view);
        glUniformMatrix3fv(spotLightTechCamWorldHandle, 1, GL_FALSE, glm::value_ptr(camWorld));

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(spotLightTechNormalHandle, 0); 
        glBindTexture(GL_TEXTURE_2D, gBuffer.mNormal);
         
        glActiveTexture(GL_TEXTURE1);
        glUniform1i(spotLightTechColorHandle, 1); 
        glBindTexture(GL_TEXTURE_2D, gBuffer.mColor); 
  
        glActiveTexture(GL_TEXTURE2);
        glUniform1i(spotLightTechDepthHandle, 2); 
        glBindTexture(GL_TEXTURE_2D, gBuffer.mDepth); 


        for(size_t i=0;i<sl.size();++i)
        {
            spotLightTech.setUniformWorldViewProj(slWVP[i]);     
            spotLightTech.setUniformWorld(slWorlds[i]);
            spotLightTech.setUniformSpotLight(sl[i]);

           // cone.draw();
        }


     
        /*
         * geometry 2
         *
         */

        qTech.use();
        qTech.setUniformViewPos(eyePos);

        gBuffer.startGeometryPass2();

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

        gBuffer.endGeometryPass2(window.getWidth(), window.getHeight());
        
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


        glDepthFunc(GL_LEQUAL); 
        cubeMapTech.use();

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(cubeMapSamplerHandle, 0); 
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTex);


        glm::mat4 cubeMapWVP = view;
        cubeMapWVP[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        cubeMapWVP = proj * cubeMapWVP;

        glUniformMatrix4fv(cubeMapWVPHandle, 1, GL_FALSE, glm::value_ptr(cubeMapWVP));

        skybox.draw();
        glDepthFunc(GL_LESS); 


        heightMapTech.use();
        glm::mat4 heightMapMtx = viewProj * heightMapScale;
        heightMapTech.setUniformWorldViewProj(heightMapMtx);
        
        glActiveTexture(GL_TEXTURE0);
        heightMapTech.setUniformSampler(0); 
        glBindTexture(GL_TEXTURE_2D, hm.mTexture);

        hm.draw();
    
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        engine.swapBuffers();
    }

    return 0;
}


