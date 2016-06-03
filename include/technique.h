#ifndef ALM_TECHNIQUE_H
#define ALM_TECHNIQUE_H


#include <vector>
#include <string>
#include <GL/glew.h> 
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "assimp/matrix4x4.h"

#include "shader.h"
#include "light.h"
#include "light_technique.h"

class Technique
{
public:
    Technique(const Shader& shader);

    Technique(const Shader& shader,
              const std::string& vertexShaderPath, 
              const std::string& fragmentShaderPath,
              bool makeAbsPaths = true);

    Technique(const std::string& vertexShaderPath, 
              const std::string& fragmentShaderPath,
              bool makeAbsPaths = true);

    virtual ~Technique();

    Technique& use();
    Technique& stop();

    Shader& getShader();
    Technique& setShader(const Shader& shader);

    Technique& setHandleWorld();
    Technique& setHandleView();
    Technique& setHandleProj();
    Technique& setHandleWorldViewProj();
    Technique& setHandleViewPos();
    Technique& setHandleSampler();
    
    Technique& setHandleDirectionalLight();
    Technique& setHandlePointLight();
    Technique& setHandleSpotLight();

    Technique& setHandleSpotLights(size_t numLights);
    Technique& setHandlePointLights(size_t numLights);
    Technique& setHandleLights(bool setHandleDirLight, size_t numSpotLights, size_t numPointLights);

    Technique& setUniformWorldViewProj(glm::mat4& worldViewProj);
    Technique& setUniformWorldViewProj(aiMatrix4x4& worldViewProj);

    Technique& setUniformWorld(glm::mat4& world);
    Technique& setUniformWorld(aiMatrix4x4& world);

    Technique& setUniformView(glm::mat4& view);
    Technique& setUniformView(aiMatrix4x4& view);

    Technique& setUniformProj(glm::mat4& proj);
    Technique& setUniformProj(aiMatrix4x4& proj);

    Technique& setUniformViewPos(const glm::vec3& vp);

    Technique& setUniformSampler(int activeTexIndex);
    
    Technique& setUniformDirectionalLight(const DirectionalLight& dirLight);
    Technique& setUniformPointLight(const PointLight& light);
    Technique& setUniformSpotLight(const SpotLight& light);

    Technique& setUniformSpotLights(const std::vector<SpotLight>& spotLights);
    Technique& setUniformPointLights(const std::vector<PointLight>& pointLights);

    Technique& setUniformLights(DirectionalLight* dirLight,
                                std::vector<SpotLight>* spotLights,
                                std::vector<PointLight>* pointLights);
    struct Uniforms {GLuint program, worldViewProj, world, view, proj, viewPos, sampler;} mUniforms;
protected:
    Technique& setUniformMatrix(GLuint handle, glm::mat4& m);
    Technique& setUniformMatrix(GLuint handle, aiMatrix4x4& m);
    Technique& setUniformVector3(GLuint handle, const glm::vec3& v);
   // Technique& setUniformVector4(GLuint handle, const glm::vec4& v);

    Technique& setUniformInt(GLuint handle, int num);
    Technique& setUniformFloat(GLuint handle, float num);

    GLuint getUniformHandle(const char* handleName); 
    GLuint getUniformHandle(const std::string& handleName); 

    

    DirectionalLightTechnique mDirLightTech;
    PointLightTechnique mPointLightTech;
    SpotLightTechnique mSpotLightTech;
    std::vector<PointLightTechnique> mPointLightTechs;
    std::vector<SpotLightTechnique> mSpotLightsTechs;
private:
    Shader mShader;

};



#endif
