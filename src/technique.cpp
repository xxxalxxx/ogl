#include "technique.h"

Technique::Technique(const Shader& shader) : mShader(shader)
{
    mUniforms.program = mShader.getProgram(); 
}

Technique::Technique(const Shader& shader, const std::string& vertexShaderPath, const std::string& fragmentShaderPath, bool makeAbsPaths /* = true */): mShader(shader)
{ 
    bool success;

    if(makeAbsPaths)
    { 
        FileSystem& fs = FileSystem::getInstance();
        success = mShader.init(fs.getAbsPath(vertexShaderPath), 
                               fs.getAbsPath(fragmentShaderPath));
    } 
    else 
    {
        success = mShader.init(vertexShaderPath, fragmentShaderPath);
    }

    if(!success)
    {
        LOG("FAILER TO LOAD SHADER");
        exit(1);
    }

    mUniforms.program = mShader.getProgram();
}

Technique::Technique(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, bool makeAbsPaths /* = false */): mShader(Shader())
{
    bool success;

    if(makeAbsPaths)
    { 
        FileSystem& fs = FileSystem::getInstance();
        success = mShader.init(fs.getAbsPath(vertexShaderPath), 
                               fs.getAbsPath(fragmentShaderPath));
    } 
    else 
    {
        success = mShader.init(vertexShaderPath, fragmentShaderPath);
    }

    if(!success)
    {
        LOG("FAILER TO LOAD SHADER");
        exit(1);
    }

    mUniforms.program = mShader.getProgram();
}


Technique::~Technique()
{

}

Technique& Technique::use()
{
    mShader.use();
    
    return *this;
}

Technique& Technique::stop()
{
    mShader.stop();
    return *this;
}

Shader& Technique::getShader()
{
    return mShader;
}

Technique& Technique::setShader(const Shader& shader)
{
    mShader = shader;
    mUniforms.program = mShader.getProgram();
    return *this;
}

Technique& Technique::setHandleWorldViewProj()
{
    mUniforms.worldViewProj = getUniformHandle("u_WVP");
    return *this;
}

Technique& Technique::setHandleWorld()
{
    mUniforms.world = getUniformHandle("u_World");
    return *this;
}

Technique& Technique::setHandleView()
{
    mUniforms.view = getUniformHandle("u_View");
    return *this;
}

Technique& Technique::setHandleProj()
{
    mUniforms.proj = getUniformHandle("u_Proj");
    return *this;
}

Technique& Technique::setHandleSampler()
{
    mUniforms.sampler = getUniformHandle("u_Sampler");
    return *this;
}

GLuint Technique::getUniformHandle(const std::string& handleName)
{
    return glGetUniformLocation(mUniforms.program, handleName.c_str());
}

GLuint Technique::getUniformHandle(const char* handleName)
{
    return glGetUniformLocation(mUniforms.program, handleName);
}

Technique& Technique::setUniformMatrix(GLuint handle, glm::mat4& m)
{
    glUniformMatrix4fv(handle, 1, GL_FALSE, glm::value_ptr(m));
    return *this;
}

Technique& Technique::setUniformMatrix(GLuint handle, aiMatrix4x4& m)
{
    glUniformMatrix4fv(handle, 1, GL_FALSE, &m.a1);
    return *this;
}

Technique& Technique::setUniformSampler(int activeTexIndex)
{
    return setUniformInt(mUniforms.sampler, activeTexIndex);
}

Technique& Technique::setUniformInt(GLuint handle, int num)
{
    glUniform1i(handle, num);
    return *this;
}

Technique& Technique::setUniformWorldViewProj(glm::mat4& worldViewProj)
{
    return setUniformMatrix(mUniforms.worldViewProj, worldViewProj);
}

Technique& Technique::setUniformWorldViewProj(aiMatrix4x4& worldViewProj)
{
    return setUniformMatrix(mUniforms.worldViewProj, worldViewProj);
}

Technique& Technique::setUniformWorld(glm::mat4& world)
{
    return setUniformMatrix(mUniforms.world, world);
}

Technique& Technique::setUniformWorld(aiMatrix4x4& world)
{
    return setUniformMatrix(mUniforms.world, world);
}

Technique& Technique::setUniformView(glm::mat4& view)
{
    return setUniformMatrix(mUniforms.view, view);
}

Technique& Technique::setUniformView(aiMatrix4x4& view)
{
    return setUniformMatrix(mUniforms.view, view);
}

Technique& Technique::setUniformProj(glm::mat4& proj)
{
    return setUniformMatrix(mUniforms.proj, proj);
}

Technique& Technique::setUniformProj(aiMatrix4x4& proj)
{
    return setUniformMatrix(mUniforms.proj, proj);
}



