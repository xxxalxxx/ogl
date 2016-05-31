#ifndef ALM_SKINNED_MODEL_H
#define ALM_SKINNED_MODEL_H

#include <math.h>

#include <map>
#include <stack>

#include "glm/gtx/string_cast.hpp"

#include "model.h"
#include "anim_node.h"
#include "skinned_model_technique.h"
#include "utils.h"

#define BONES_PER_VERTEX 4

class SkinnedModel : public Model 
{
public:

    struct BoneTransform
    {
        aiMatrix4x4 mBoneSpaceTransform, mAnimatedTransform;
    };


    struct Bone
    {
        glm::ivec4 mBoneIndices;
        glm::vec4 mWeights;
    };


    SkinnedModel(
            const std::string& fileName, 
            const std::string& texturesDir,
            unsigned int modelFlags = 0,
            int aiProcessArgs = DEFAULT_AIPROCESS_ARGS);

    SkinnedModel(
            const std::string& fileName, 
            const char* texturesDir = NULL, 
            unsigned int modelFlags = 0,
            int aiProcessArgs = DEFAULT_AIPROCESS_ARGS);

    ~SkinnedModel();


    bool init();
        
    void update(float currTime, const std::string& animationName);
    void update(float currTime, unsigned int animationIndex = 0);
    void draw(SkinnedModelTechnique& tech);
    
    size_t mNumBones;

private:

    typedef std::map<std::string, unsigned int> sk_model_container_t;
    typedef sk_model_container_t::iterator sk_model_iter_t;
    typedef std::pair<std::string, unsigned int> sk_model_pair_t;
    
    typedef std::map<aiNode*, int> sk_nodes_container_t;
    typedef sk_nodes_container_t::iterator sk_nodes_iter_t;
    typedef std::pair<aiNode*, unsigned int> sk_nodes_pair_t;

    void createAnimNode(const aiScene* scene, aiNode* node, AnimNode* parent, sk_nodes_container_t& animNodeChecklist);
    
    void processMeshBones(const aiScene* scene, aiNode* node, aiMesh* mesh, size_t baseVertex, std::vector<Bone>& bones,
                          sk_model_container_t& boneNameIndices, sk_nodes_container_t& animNodeChecklist);

    void processAnimNode(float progress, aiMatrix4x4& parentTransform, AnimNode* animNode, unsigned int animationIndex);


    unsigned int getUpdatedBoneIndex(sk_model_container_t& boneNameIndices, const char* boneName);

    void initBuffers(std::vector<Vertex>& vertices,  std::vector<GLuint>& indices,  std::vector<Bone>& bones);
    void initBones(std::vector<Bone>& bones);
   
    void unload();
    void deleteAnimNodes(AnimNode* node);


    const aiScene* mScene;
    Assimp::Importer importer;
    
    AnimNode* mAnimNodeRoot;

    sk_model_container_t mAnimationNameIndices;

    std::vector<BoneTransform> mBoneTransforms;
    aiMatrix4x4 mModelSpaceTransform;
    
    GLuint mBoneBuffer;  
   
};

#endif 
