#ifndef ALM_TECHNIQUE
#define ALM_TECHNIQUE

#include <GL/glew.h> 

#include "glm/mat4x4.hpp"
#include "assimp/matrix4x4.h"

#include "Shader.h"

class Technique
{
public:
    Technique(const Shader& shader)

    virtual ~Technique();

    void use() const;

    Shader& getShader() const;
    Technique& setShader(const Shader& shader);

    GLuint getWorldViewProjHandle();
    GLuint getWorldHandle();
    GLuint getViewHandle();
    GLuint getProjHandle();

    Technique& setWorldViewProj(glm::mat4& worldViewProj);
    Technique& setWorldViewProj(aiMatrix4x4& worldViewProj);

    Technique& setWorldProj(glm::mat4& world);
    Technique& setWorld(aiMatrix4x4& world);

    Technique& setView(glm::mat4& view);
    Technique& setView(aiMatrix4x4& view);

    Technique& setProj(glm::mat4& proj);
    Technique& setProj(aiMatrix4x4& proj);

    const char* getTextureName(aiTextureType t) const;
 

protected:
    Technique& setUniformMatrix(GLuint handle, glm::mat4& m);
    Technique& setUniformMatrix(GLuint handle, aiMatrix4x4& m);

    GLuint getUniformHandle(const char* handleName); 

    struct Uniforms {GLuint program, worldViewProj, world, view, proj;} mUniforms;
private:
    Shader mShader;
    static const char * const mTextureTypeNames;
    static const aiTextureType mTextureTypes;
    static const size_t mNumTextureTypes;
};


// TODO: mode these to mesh technique
inline const char * const Technique::mTextureTypeNames[] = {
    "", "u_TextureDiffuse", "u_TextureSpecular", "u_TextureAmbient",
    "u_TextureEmissive", "u_TextureHeight", "u_TextureNormals", 
    "u_TextureShininess", "u_TextureOpacity", "u_TextureDisplacement",
    "u_TextureLightMap", "u_TextureReflection", "" 
};

inline const size_t mNumTextureTypes = sizeof(Technique::mNumTextureTypeNames)/sizeof(Technique::mNumTextureTypeNames[0]);

inline const char* getTextureName(aiTextureType t) const
{
    return mTextureTypes[t];
}

// end TODO

inline Technique::Technique(const Shader& shader) : mShader(shader)
{
    mUniforms.program = mShader.getProgram(); 
}

inline Technique::~Technique()
{
}

inline void Technique::use() const
{
    mShader.use();
}

inline Shader& Technique::getShader() const
{
    return mShader;
}

inline Technique& Technique::setShader(const Shader& shader)
{
    mShader = shader;
    mUniforms.program = shader.getProgram();
    return *this;
}


inline GLuint getUniformHandle(const char* handleName)
{
    return glGetUniformLocation(mUniforms.program, handleName);

inline Technique& setUniformMatrix(GLuint handle, glm::mat4& m)
{
    glUniformMatrix4fv(handle, glm::value_ptr(m));
    return *this;
}

inline Technique& setUniformMatrix(GLuint handle, aiMatrix4x4& m)
{
    glUniformMatrix4fv(handle, &m.a1);
    return *this;
}


inline Technique& setWorldViewProj(glm::mat4& worldViewProj)
{
    return setUniformMatrix(mUniforms.worldViewProj, worldViewProj);
}

inline Technique& setWorldViewProj(aiMatrix4x4& worldViewProj)
{
    return setUniformMatrix(mUniforms.worldViewProj, worldViewProj);
}


inline Technique& setWorld(glm::mat4& world)
{
    return setUniformMatrix(mUniforms.world, world);
}

inline Technique& setWorld(aiMatrix4x4& world)
{
    return setUniformMatrix(mUniforms.world, world);
}


inline Technique& setView(glm::mat4& view)
{
    return setUniformMatrix(mUniforms.view, view);
}

inline Technique& setView(aiMatrix4x4& view)
{
    return setUniformMatrix(mUniforms.view, view);
}


inline Technique& setProj(glm::mat4& proj)
{
    return setUniformMatrix(mUniforms.proj, proj);
}

inline Technique& setProj(aiMatrix4x4& proj)
{
    return setUniformMatrix(mUniforms.proj, proj);
}






#endif
