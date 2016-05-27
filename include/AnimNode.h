#ifndef ALM_ANIM_NODE_H
#define ALM_ANIM_NODE_H

#include <math.h>

#include <vector>

#include "glm/vec4.hpp"

#include "Mesh.h"


struct AnimNode 
{
    const static double DEFAULT_TICKS_PER_SECOND;
    const static double DEFAULT_TICKS_DURATION;

    AnimNode(aiNode& assimpNode, AnimNode* parent = NULL);
    ~AnimNode();

    bool hasRelatedBone() const;

    bool isRoot() const;

    bool isAnimatedAtIndex(unsigned int animIndex) const;

    aiMatrix4x4 getAnimatedTransform(float progress, unsigned int animIndex);
    
    aiNode& mAssimpNode;
    
    AnimNode* mParent;

    int mBoneTransformIndex;

    std::vector<AnimNode*> mChildren;
    std::vector<aiNodeAnim*> mAnimTypes;

private:
    aiMatrix4x4 getTranslation(float progress, unsigned int animIndex, aiNodeAnim& channel);
    aiMatrix4x4 getRotation(float progress, unsigned int animIndex, aiNodeAnim& channel);
    aiMatrix4x4 getScaling(float progress, unsigned int animIndex, aiNodeAnim& channel);
};

#endif
