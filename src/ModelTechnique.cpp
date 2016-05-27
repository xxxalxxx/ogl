#include "ModelTechnique.h"

const char * const ModelTechnique::mTextureTypeNames[] = {
    "", "u_TextureDiffuse", "u_TextureSpecular", "u_TextureAmbient",
    "u_TextureEmissive", "u_TextureHeight", "u_TextureNormals", 
    "u_TextureShininess", "u_TextureOpacity", "u_TextureDisplacement",
    "u_TextureLightMap", "u_TextureReflection", "" 
};


ModelTechnique::ModelTechnique(const Shader& shader): Technique(shader)
{

}

ModelTechnique::ModelTechnique(const Shader& shader,
                                const std::string& vertexShaderPath,
                                const std::string& fragmentShaderPath): 
    Technique(shader, vertexShaderPath, fragmentShaderPath)
{
}

ModelTechnique::~ModelTechnique()
{       
}

void ModelTechnique::addMaterialTextures(const std::vector<Texture>& textures)
{
    mMaterialHandles.push_back(std::vector<GLuint>());
    size_t texIndices[NUM_TEXTURE_TYPES] = {0};

    for(size_t i=0;i<textures.size();++i)
    {  

        size_t index = static_cast<size_t>(textures[i].type);// static_cast<typename std::underlying_type<aiTextureType>::type>(textures[i].type);
        std::string handleStr(mTextureTypeNames[index]);
        handleStr.append('[' + std::to_string(texIndices[index]++) + ']');

        GLuint handle = getUniformHandle(handleStr);
        mMaterialHandles.back().push_back(handle);
        LOG("HANDLE:" << handleStr);
    }
}

ModelTechnique& ModelTechnique::setUniformSampler(size_t materialIndex, GLuint handleIndex)
{
    std::vector<GLuint>& samplerHandles = mMaterialHandles[materialIndex];
    if(samplerHandles.size() > handleIndex)
        glUniform1i(samplerHandles[handleIndex], handleIndex);
    return *this;
}

ModelTechnique& ModelTechnique::setHandleMaterials(std::vector<std::vector<Texture> >& materials)
{
    mMaterialHandles.clear();

    for(size_t i=0;i<materials.size();++i)
    {
        addMaterialTextures(materials[i]);
    }
    return *this;
}


