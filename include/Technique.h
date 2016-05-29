#ifndef ALM_TECHNIQUE_H
#define ALM_TECHNIQUE_H

#include <GL/glew.h> 

#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "assimp/matrix4x4.h"

#include "Shader.h"

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
    Technique& setHandleSampler();

    Technique& setUniformWorldViewProj(glm::mat4& worldViewProj);
    Technique& setUniformWorldViewProj(aiMatrix4x4& worldViewProj);

    Technique& setUniformWorld(glm::mat4& world);
    Technique& setUniformWorld(aiMatrix4x4& world);

    Technique& setUniformView(glm::mat4& view);
    Technique& setUniformView(aiMatrix4x4& view);

    Technique& setUniformProj(glm::mat4& proj);
    Technique& setUniformProj(aiMatrix4x4& proj);

    Technique& setUniformSampler(int activeTexIndex);

protected:
    Technique& setUniformMatrix(GLuint handle, glm::mat4& m);
    Technique& setUniformMatrix(GLuint handle, aiMatrix4x4& m);
    Technique& setUniformInt(GLuint handle, int num);

    GLuint getUniformHandle(const char* handleName); 
    GLuint getUniformHandle(const std::string& handleName); 

    struct Uniforms {GLuint program, worldViewProj, world, view, proj, sampler;} mUniforms;

private:
    Shader mShader;

};



#endif
