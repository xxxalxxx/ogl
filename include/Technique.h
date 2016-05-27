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
              const std::string& fragmentShaderPath);
    virtual ~Technique();

    Technique& use();

    Shader& getShader();
    Technique& setShader(const Shader& shader);

    Technique& setHandleWorld();
    Technique& setHandleView();
    Technique& setHandleProj();
    Technique& setHandleWorldViewProj();

    Technique& setUniformWorldViewProj(glm::mat4& worldViewProj);
    Technique& setUniformWorldViewProj(aiMatrix4x4& worldViewProj);

    Technique& setUniformWorld(glm::mat4& world);
    Technique& setUniformWorld(aiMatrix4x4& world);

    Technique& setUniformView(glm::mat4& view);
    Technique& setUniformView(aiMatrix4x4& view);

    Technique& setUniformProj(glm::mat4& proj);
    Technique& setUniformProj(aiMatrix4x4& proj);

protected:
    Technique& setUniformMatrix(GLuint handle, glm::mat4& m);
    Technique& setUniformMatrix(GLuint handle, aiMatrix4x4& m);
    Technique& setUniformInt(GLuint handle, int num);

    GLuint getUniformHandle(const char* handleName); 
    GLuint getUniformHandle(const std::string& handleName); 

    struct Uniforms {GLuint program, worldViewProj, world, view, proj;} mUniforms;
private:
    Shader mShader;

};


inline Technique::Technique(const Shader& shader) : mShader(shader)
{
    mUniforms.program = mShader.getProgram(); 
}
inline Technique::Technique(const Shader& shader, const std::string& vertexShaderPath, const std::string& fragmentShaderPath): mShader(shader)
{
    if(!mShader.init(vertexShaderPath, fragmentShaderPath))
    {
        LOG("FAILER TO LOAD SHADER");
        exit(1);
    }

    mUniforms.program = mShader.getProgram();
}

inline Technique::~Technique()
{

}

inline Technique& Technique::use()
{
    mShader.use();
    return *this;
}

inline Shader& Technique::getShader()
{
    return mShader;
}

inline Technique& Technique::setShader(const Shader& shader)
{
    mShader = shader;
    mUniforms.program = mShader.getProgram();
    return *this;
}

inline Technique& Technique::setHandleWorldViewProj()
{
    mUniforms.worldViewProj = getUniformHandle("u_WVP");
    return *this;
}

inline Technique& Technique::setHandleWorld()
{
    mUniforms.world = getUniformHandle("u_World");
    return *this;
}

inline Technique& Technique::setHandleView()
{
    mUniforms.view = getUniformHandle("u_View");
    return *this;
}

inline Technique& Technique::setHandleProj()
{
    mUniforms.proj = getUniformHandle("u_Proj");
    return *this;
}

inline GLuint Technique::getUniformHandle(const std::string& handleName)
{
    return glGetUniformLocation(mUniforms.program, handleName.c_str());
}

inline GLuint Technique::getUniformHandle(const char* handleName)
{
    return glGetUniformLocation(mUniforms.program, handleName);
}
inline Technique& Technique::setUniformMatrix(GLuint handle, glm::mat4& m)
{
    glUniformMatrix4fv(handle, 1, GL_FALSE, glm::value_ptr(m));
    return *this;
}

inline Technique& Technique::setUniformMatrix(GLuint handle, aiMatrix4x4& m)
{
    glUniformMatrix4fv(handle, 1, GL_FALSE, &m.a1);
    return *this;
}

inline Technique& Technique::setUniformInt(GLuint handle, int num)
{
    glUniform1i(handle, num);
    return *this;
}

inline Technique& Technique::setUniformWorldViewProj(glm::mat4& worldViewProj)
{
    return setUniformMatrix(mUniforms.worldViewProj, worldViewProj);
}

inline Technique& Technique::setUniformWorldViewProj(aiMatrix4x4& worldViewProj)
{
    return setUniformMatrix(mUniforms.worldViewProj, worldViewProj);
}


inline Technique& Technique::setUniformWorld(glm::mat4& world)
{
    return setUniformMatrix(mUniforms.world, world);
}

inline Technique& Technique::setUniformWorld(aiMatrix4x4& world)
{
    return setUniformMatrix(mUniforms.world, world);
}


inline Technique& Technique::setUniformView(glm::mat4& view)
{
    return setUniformMatrix(mUniforms.view, view);
}

inline Technique& Technique::setUniformView(aiMatrix4x4& view)
{
    return setUniformMatrix(mUniforms.view, view);
}


inline Technique& Technique::setUniformProj(glm::mat4& proj)
{
    return setUniformMatrix(mUniforms.proj, proj);
}

inline Technique& Technique::setUniformProj(aiMatrix4x4& proj)
{
    return setUniformMatrix(mUniforms.proj, proj);
}






#endif
