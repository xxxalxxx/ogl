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

Technique& Technique::setHandleViewPos()
{
    mUniforms.viewPos = getUniformHandle("u_ViewPos");
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

Technique& Technique::setUniformVector(GLuint handle, const glm::vec3& v)
{
    glUniform3fv(handle, 1, glm::value_ptr(v));
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

Technique& Technique::setUniformFloat(GLuint handle, float num)
{
    glUniform1f(handle, num);
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

Technique& Technique::setUniformViewPos(const glm::vec3& vp)
{
    return setUniformVector(mUniforms.viewPos, vp);
}

Technique& Technique::setHandleDirectionalLight()
{
    mDirLightTech.handleDirection = getUniformHandle("u_DirLight.direction");

    mDirLightTech.handleAmbient  = getUniformHandle("u_DirLight.ambient");
    mDirLightTech.handleSpecular = getUniformHandle("u_DirLight.specular");
    mDirLightTech.handleDiffuse  = getUniformHandle("u_DirLight.diffuse");

    return *this;
}

Technique& Technique::setHandleSpotLights(size_t numLights)
{
    mSpotLightsTechs.clear();

    for(size_t i=0;i<numLights;++i)
    {
        std::string name("u_SpotLights[" + std::to_string(i) + "].");

        mSpotLightsTechs.push_back(SpotLightTechnique());
        SpotLightTechnique& spot = mSpotLightsTechs.back();

        spot.handlePosition  = getUniformHandle( name + "position");
        spot.handleDirection = getUniformHandle( name + "direction");

        spot.handleCutoff      = getUniformHandle(name + "cutoff");
        spot.handleCutoffStart = getUniformHandle(name + "cutoffStart");       
        spot.handleExponent    = getUniformHandle(name + "exponent");

        spot.handleA0 = getUniformHandle(name + "a0");
        spot.handleA1 = getUniformHandle(name + "a1");
        spot.handleA2 = getUniformHandle(name + "a2");

        spot.handleAmbient  = getUniformHandle(name + "ambient");
        spot.handleSpecular = getUniformHandle(name + "specular");
        spot.handleDiffuse  = getUniformHandle(name + "diffuse");
    }

    return *this;
}

Technique& Technique::setHandlePointLights(size_t numLights)
{
    mPointLightTechs.clear();

    for(size_t i=0;i<numLights;++i)
    {
        std::string name("u_PointLights[" + std::to_string(i) + "].");

        mPointLightTechs.push_back(PointLightTechnique());
        PointLightTechnique& point = mPointLightTechs.back();

        point.handlePosition = getUniformHandle( name + "position");

        point.handleA0 = getUniformHandle(name + "a0");
        point.handleA1 = getUniformHandle(name + "a1");
        point.handleA2 = getUniformHandle(name + "a2");

        point.handleAmbient  = getUniformHandle(name + "ambient");
        point.handleSpecular = getUniformHandle(name + "specular");
        point.handleDiffuse  = getUniformHandle(name + "diffuse");
 
    }

    return *this;

}

Technique& Technique::setUniformDirectionalLight(const DirectionalLight& dirLight)
{
    setUniformVector(mDirLightTech.handleDirection, dirLight.direction);
    setUniformVector(mDirLightTech.handleAmbient, dirLight.ambient);
    setUniformVector(mDirLightTech.handleSpecular, dirLight.specular);
    setUniformVector(mDirLightTech.handleDiffuse, dirLight.diffuse);

    return *this;
}

Technique& Technique::setHandleLights(bool setHandleDirLight, size_t numSpotLights, size_t numPointLights)
{
    if(setHandleDirLight) setHandleDirectionalLight();
    if(numSpotLights) setHandleSpotLights(numSpotLights);
    if(numPointLights) setHandlePointLights(numPointLights);

    return *this;
}
Technique& Technique::setUniformSpotLights(const std::vector<SpotLight>& spotLights)
{
    size_t len = spotLights.size() < mSpotLightsTechs.size()
               ? spotLights.size() : mSpotLightsTechs.size();

    for(size_t i=0;i<len;++i)
    {
        const SpotLight& spot = spotLights[i];
        SpotLightTechnique& spotTech = mSpotLightsTechs[i];

        setUniformVector(spotTech.handleDirection, spot.direction);
        setUniformVector(spotTech.handlePosition, spot.position);
   
        setUniformFloat(spotTech.handleCutoff, spot.cutoff);
        setUniformFloat(spotTech.handleCutoffStart, spot.cutoffStart);
        setUniformFloat(spotTech.handleExponent, spot.exponent);

        setUniformFloat(spotTech.handleA0, spot.a0);
        setUniformFloat(spotTech.handleA1, spot.a1);
        setUniformFloat(spotTech.handleA2, spot.a2);

        setUniformVector(spotTech.handleAmbient, spot.ambient);
        setUniformVector(spotTech.handleSpecular, spot.ambient);
        setUniformVector(spotTech.handleDiffuse, spot.diffuse);
    }

    return *this;
}

Technique& Technique::setUniformPointLights(const std::vector<PointLight>& pointLights)
{
    size_t len = pointLights.size() < mPointLightTechs.size()
               ? pointLights.size() : mPointLightTechs.size();

    for(size_t i=0;i<len;++i)
    {
        const PointLight& point = pointLights[i];
        PointLightTechnique& pointTech = mPointLightTechs[i];

        setUniformVector(pointTech.handlePosition, point.position);
   
        setUniformFloat(pointTech.handleA0, point.a0);
        setUniformFloat(pointTech.handleA1, point.a1);
        setUniformFloat(pointTech.handleA2, point.a2);

        setUniformVector(pointTech.handleAmbient, point.ambient);
        setUniformVector(pointTech.handleSpecular, point.ambient);
        setUniformVector(pointTech.handleDiffuse, point.diffuse);
    }

    return *this;
}

Technique& Technique::setUniformLights(DirectionalLight* dirLight,
                                        std::vector<SpotLight>* spotLights,
                                        std::vector<PointLight>* pointLights)
{
    if(dirLight) setUniformDirectionalLight(*dirLight);
    if(spotLights) setUniformSpotLights(*spotLights);
    if(pointLights) setUniformPointLights(*pointLights);
    return *this;
}


