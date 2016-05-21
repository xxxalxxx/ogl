#include "model.h"
#include "skinned_mesh.h"
#include <map>


class SkinnedModel : public Model 
{
public:
    std::string mAnimationsPath;
    const aiScene* mScene;
    std::map<const char*, unsigned int> mAnimationNameIndicies, mBoneNameIndices;
    aiMatrix4x4 mRootNodeInverseTransform;
    unsigned int mNumBones;
    SkinnedModel(
            const char* path, 
            const char* texturesDir,
            Shader& shader,   
            const char* animationsPath = NULL,
            int aiProcessArgs = 0, 
            bool gamma = false
            ): Model(path, texturesDir, shader, aiProcessArgs, gamma), mNumBones(0)
    {   
        if(animationsPath)
            mAnimationsPath = animationsPath;
        else
            mAnimationsPath = path;
        
        
    
    }
    
    bool init()
    {
        Assimp::Importer importer;
        std::string& path = Model::mPath;
        int aiProcessArgs = Model::mAiProcessArgs;

        mScene = importer.ReadFile(
                path, 
                aiProcessArgs ? aiProcessArgs :
                  aiProcess_Triangulate 
                | aiProcess_FlipUVs 
                | aiProcess_CalcTangentSpace
                );
        // Check for errors
        if(!mScene || mScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !mScene->mRootNode) // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return false;
        }

        mRootNodeInverseTransform = mScene->mRootNode->mTransformation;
        mRootNodeInverseTransform.Inverse();       

        if(mScene->HasMeshes())
        {
            processNode(mScene, mScene->mRootNode);
        }

        if(mScene->HasAnimations())
        {
            for(size_t i=0;i<mScene->mNumAnimations;++i)
            {
                LOG(mScene->mAnimations[i]->mName.C_Str());
                const char* animName = mScene->mAnimations[i]->mName.C_Str();
                mAnimationNameIndicies.insert(std::pair<const char*, unsigned int>(animName, i));
            }
        }
        else
        {
            LOG("NO ANIMATIONS FOUND");
            return false;
        }

        return true;


    }

    void processNode(const aiScene* scene, aiNode* node)
    {
        LOG(node->mName.C_Str());
        
        for(size_t i=0;i<node->mNumMeshes;++i)
        {
            processMesh(scene, scene->mMeshes[node->mMeshes[i]]); 
        }
        
        for(size_t i=0;i<node->mNumChildren;++i)
        {
            processNode(scene, node->mChildren[i]);
        }


    }

    void processMesh(const aiScene* scene, aiMesh* mesh)
    {   
        Mesh processedMesh = Model::processMesh(scene, mesh, Model::mShader);
        std::vector< std::vector<BoneWeight> > boneWeights;
        boneWeights.resize(mesh->mNumVertices); // num weights = num vertices
        
        std::vector<Bone> boneTransforms; 

        if(mesh->HasBones()) 
        {  
            for(size_t i=0;i<mesh->mNumBones;++i)
            {
                aiBone* bone = mesh->mBones[i];
                const char* boneName = bone->mName.C_Str(); 
                
                LOG(boneName);
/*              unsigned int boneIndex;

                std::map<const char*, unsigned int>::iterator 
                it = mBoneNameIndices.find(boneName);
                
                if(it == mBoneNameIndices.end())
                {
                    boneIndex = mNumBones++;
                    boneTransforms.push_back(BoneTransform());
                    mBoneNameIndices[boneName] = boneIndex;
                }
                else 
                {
                    boneIndex = it->second;
                }
*/
                boneTransforms[i].boneSpaceTransform = mesh->mBones[i]->mOffsetMatrix;
                            
                for(size_t j=0;j<bone->mNumWeights;++j)
                {
                    aiVertexWeight& vertexWeight = bone->mWeights[j];
                    BoneWeight boneWeight(i, vertexWeight.mWeight);
                    boneWeights[vertexWeight.mVertexId].push_back(boneWeight);
                }
            }
            
            //set model here


        }
    }
  


    void processAnimations(const aiScene* scene, unsigned int animationNameIndex = 0)
    {
        if(animationNameIndex < scene->mNumAnimations) 
        {
            aiAnimation* animation = scene->mAnimations[animationNameIndex];

            for(size_t j=0;j<animation->mNumChannels;++j)
            {
                aiNodeAnim* nodeAnim = animation->mChannels[j];
            }
        }     
    }

    void update(float dt)
    {
    
    }

private:
    


};