#ifndef ALM_SKINNED_MODEL_TECHNIQUE_H
#define ALM_SKINNED_MODEL_TECHNIQUE_H

#include "ModelTechnique.h"

class SkinnedModelTechnique : public ModelTechnique 
{
public:
    SkinnedModelTechnique(const Shader& shader);

    SkinnedModelTechnique(const Shader& shader, 
            const std::string& vertexShaderPath,
            const std::string& fragmentShaderPath,
            bool makeAbsPaths = true);

    SkinnedModelTechnique(const std::string& vertexShaderPath,
                          const std::string& fragmentShaderPath,
                          bool makeAbsPaths = true);

    ~SkinnedModelTechnique();

    SkinnedModelTechnique& setUniformBoneTransform(size_t boneIndex, glm::mat4&   m);
    SkinnedModelTechnique& setUniformBoneTransform(size_t boneIndex, aiMatrix4x4& m);
    SkinnedModelTechnique& setHandleBoneTransforms(size_t numBones);
private:
    std::vector<GLuint> mBoneHandles;
};


#endif
