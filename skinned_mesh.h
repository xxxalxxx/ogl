#include "mesh.h"


struct Bone
{
    aiMatrix4x4 boneSpaceTransform, animatedTransform;
};

struct BoneWeight
{
    BoneWeight(size_t boneIndex, float weight) 
        : boneIndex(boneIndex), weight(weight)
    {
    }
    
    size_t boneIndex;
    float weight;
};


class SkinnedMesh
{
public:
    SkinnedMesh(
            std::vector<Vertex>& vertices, 
            std::vector<GLuint>& indices, 
            std::vector<Texture>& textures, 
            const Shader& shader
            ) : mMesh(vertices, indices, textures, shader)
    {
    
    }

    SkinnedMesh(const std::vector<BoneWeight>& boneWeights, const Mesh& mesh): mMesh(mesh)
    {
    }

    Mesh mMesh;

};