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

Technique& Technique::setUniformVector3(GLuint handle, const glm::vec3& v)
{
    glUniform3fv(handle, 1, glm::value_ptr(v));
    return *this;
}
/*
Technique& Technique::setUniformVector4(GLuint handle, const glm::vec4& v)
{
    glUniform4fv(handle, 1, glm::value_ptr(v));
    return *this;
}
*/
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
    return setUniformVector3(mUniforms.viewPos, vp);
}

Technique& Technique::setHandleDirectionalLight()
{
    mDirLightTech.handleDirection = getUniformHandle("u_DirLight.direction");

    mDirLightTech.handleColor = getUniformHandle("u_DirLight.color");
    mDirLightTech.handleSpecPower = getUniformHandle("u_DirLight.specPower");
//  mDirLightTech.handleAmbient  = getUniformHandle("u_DirLight.ambient");
//    mDirLightTech.handleSpecular = getUniformHandle("u_DirLight.specular");
//    mDirLightTech.handleDiffuse  = getUniformHandle("u_DirLight.diffuse");

    return *this;
}

Technique& Technique::setHandlePointLight()
{
    std::string name("light.");

    mPointLightTech.handlePosition = getUniformHandle( name + "position");

    mPointLightTech.handleQuadratic = getUniformHandle(name + "quadratic");
    mPointLightTech.handleLinear = getUniformHandle(name + "linear");
    mPointLightTech.handleConstant = getUniformHandle(name + "constant");

    mPointLightTech.handleColor = getUniformHandle(name + "color");

    return *this;
}


Technique& Technique::setHandleSpotLight()
{
    std::string name("light.");

    mSpotLightTech.handlePosition = getUniformHandle( name + "position");
    mSpotLightTech.handleDirection = getUniformHandle( name + "direction");
    
    mSpotLightTech.handleCutoff      = getUniformHandle(name + "cutoff");
    mSpotLightTech.handleCutoffStart = getUniformHandle(name + "cutoffStart");       
    mSpotLightTech.handleExponent    = getUniformHandle(name + "exponent");


    mSpotLightTech.handleQuadratic = getUniformHandle(name + "quadratic");
    mSpotLightTech.handleLinear = getUniformHandle(name + "linear");
    mSpotLightTech.handleConstant = getUniformHandle(name + "constant");

    mSpotLightTech.handleColor = getUniformHandle(name + "color");

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

        spot.handleQuadratic = getUniformHandle(name + "quadratic");
        spot.handleLinear = getUniformHandle(name + "linear");
        spot.handleConstant = getUniformHandle(name + "constant");

        spot.handleColor = getUniformHandle(name + "color");
        //     spot.handleAmbient  = getUniformHandle(name + "ambient");
    //    spot.handleSpecular = getUniformHandle(name + "specular");
    //    spot.handleDiffuse  = getUniformHandle(name + "diffuse");

        
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

        point.handleQuadratic = getUniformHandle(name + "quadratic");
        point.handleLinear = getUniformHandle(name + "linear");
        point.handleConstant = getUniformHandle(name + "constant");

        point.handleColor = getUniformHandle(name + "color");

/*
        point.handleAmbient  = getUniformHandle(name + "ambient");
        point.handleSpecular = getUniformHandle(name + "specular");
        point.handleDiffuse  = getUniformHandle(name + "diffuse");
*/
 
    }

    return *this;

}

Technique& Technique::setUniformDirectionalLight(const DirectionalLight& dirLight)
{
    setUniformVector3(mDirLightTech.handleDirection, dirLight.direction);
    setUniformVector3(mDirLightTech.handleColor, dirLight.color);

    return *this;
}

Technique& Technique::setUniformPointLight(const PointLight& point)
{
    setUniformVector3(mPointLightTech.handlePosition, point.position);
   
    setUniformFloat(mPointLightTech.handleQuadratic, point.quadratic);
    setUniformFloat(mPointLightTech.handleLinear, point.linear);
    setUniformFloat(mPointLightTech.handleConstant, point.constant);

    setUniformVector3(mPointLightTech.handleColor, point.color);

    setUniformFloat(mPointLightTech.handleSpecPower, point.specPower);
   
    return *this;
}



Technique& Technique::setUniformSpotLight(const SpotLight& light)
{
    setUniformVector3(mSpotLightTech.handlePosition, light.position);
    setUniformVector3(mSpotLightTech.handleDirection, light.direction);
    
    setUniformFloat(mSpotLightTech.handleCutoff, light.cutoff);
    setUniformFloat(mSpotLightTech.handleCutoffStart, light.cutoffStart);
    setUniformFloat(mSpotLightTech.handleExponent, light.exponent);

    setUniformFloat(mSpotLightTech.handleQuadratic, light.quadratic);
    setUniformFloat(mSpotLightTech.handleLinear, light.linear);
    setUniformFloat(mSpotLightTech.handleConstant, light.constant);

    setUniformVector3(mSpotLightTech.handleColor, light.color);

    setUniformFloat(mSpotLightTech.handleSpecPower, light.specPower);
   
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

        setUniformVector3(spotTech.handleDirection, spot.direction);
        setUniformVector3(spotTech.handlePosition, spot.position);
   
        setUniformFloat(spotTech.handleCutoff, spot.cutoff);
        setUniformFloat(spotTech.handleCutoffStart, spot.cutoffStart);
        setUniformFloat(spotTech.handleExponent, spot.exponent);

        setUniformFloat(spotTech.handleQuadratic, spot.quadratic);
        setUniformFloat(spotTech.handleLinear, spot.linear);
        setUniformFloat(spotTech.handleConstant, spot.constant);

        setUniformVector3(spotTech.handleColor, spot.color);


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

        setUniformVector3(pointTech.handlePosition, point.position);
   
        setUniformFloat(pointTech.handleQuadratic, point.quadratic);
        setUniformFloat(pointTech.handleLinear, point.linear);
        setUniformFloat(pointTech.handleConstant, point.constant);


        setUniformVector3(pointTech.handleColor, point.color);

        setUniformFloat(pointTech.handleSpecPower, point.specPower);

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


