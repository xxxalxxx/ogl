#include "SkinnedModelTechnique.h"

SkinnedModelTechnique::SkinnedModelTechnique(const Shader& shader): ModelTechnique(shader) 
{

}

SkinnedModelTechnique::SkinnedModelTechnique(const Shader& shader, 
                                            const std::string& vertexShaderPath, 
                                            const std::string& fragmentShaderPath,
                                            bool makeAbsPaths /* = true */): ModelTechnique(shader, vertexShaderPath, fragmentShaderPath, makeAbsPaths)
{

}

SkinnedModelTechnique::SkinnedModelTechnique(const std::string& vertexShaderPath,
                                             const std::string& fragmentShaderPath,
                                             bool makeAbsPaths /* = true */): ModelTechnique(vertexShaderPath, fragmentShaderPath, makeAbsPaths)
{

}

SkinnedModelTechnique::~SkinnedModelTechnique()
{

}


SkinnedModelTechnique& SkinnedModelTechnique::setUniformBoneTransform(size_t boneIndex, glm::mat4& m)
{
    setUniformMatrix(mBoneHandles[boneIndex], m);
    return *this;
}

SkinnedModelTechnique& SkinnedModelTechnique::setUniformBoneTransform(size_t boneIndex, aiMatrix4x4& m)
{
    setUniformMatrix(mBoneHandles[boneIndex], m);
    return *this;
}

SkinnedModelTechnique& SkinnedModelTechnique::setHandleBoneTransforms(size_t numBones)
{
    mBoneHandles.clear();
    
    for(size_t i=0;i<numBones;++i)
    {
        std::string handleStr("u_Bones[" + std::to_string(i) + "]");
        GLuint handle = getUniformHandle(handleStr.c_str());
        mBoneHandles.push_back(handle);
    }

    return *this;
}

