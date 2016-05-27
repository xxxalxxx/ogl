#include <math.h>

#include <map>
#include <stack>

#include "glm/gtx/string_cast.hpp"

#include "Model.h"
#include "AnimNode.h"

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


    const aiScene* mScene;
    Assimp::Importer importer;

    typedef std::map<std::string, unsigned int> sk_model_container_t;
    typedef sk_model_container_t::iterator sk_model_iter_t;
    typedef std::pair<std::string, unsigned int> sk_model_pair_t;
    
    typedef std::map<aiNode*, int> sk_nodes_container_t;
    typedef sk_nodes_container_t::iterator sk_nodes_iter_t;
    typedef std::pair<aiNode*, unsigned int> sk_nodes_pair_t;
    
    AnimNode* mAnimNodeRoot;

    sk_model_container_t mAnimationNameIndices;

    std::vector<BoneTransform> mBoneTransforms;
    aiMatrix4x4 mModelSpaceTransform;

    size_t mNumBones;

    
    SkinnedModel(
            const std::string& fileName, 
            Shader& shader,   
            const std::string& texturesDir,
            int aiProcessArgs = 0, 
            bool gamma = false
            ): SkinnedModel(fileName, shader, texturesDir.c_str(), aiProcessArgs, gamma)
    {   
    }

    SkinnedModel(
            const std::string& fileName, 
            Shader& shader,   
            const char* texturesDir = NULL,
            int aiProcessArgs = 0, 
            bool gamma = false
            ): Model(fileName, shader, texturesDir, aiProcessArgs, gamma), mScene(NULL), mNumBones(0)
    {   
    }

    ~SkinnedModel()
    {
        LOG("DESTR");
        if(mAnimNodeRoot)
        {
            deleteAnimNodes(mAnimNodeRoot);
        }
        unload();
    }


    bool init()
    {
        mScene = importer.ReadFile(
                mFileName,
                mAiProcessArgs ? mAiProcessArgs :
                  aiProcess_Triangulate 
               // | aiProcess_FlipUVs 
                | aiProcess_CalcTangentSpace
                );
        // Check for errors
        if(!mScene || mScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !mScene->mRootNode) // if is Not Zero
        {
            LOG( "ERROR::ASSIMP:: " << importer.GetErrorString() );
            return false;
        }

 
        if(!mScene->HasAnimations())
        {
            LOG("ERROR: NO ANIMATIONS FOUND, USE MESH INSTEAD");
            return false;
        }

        if(!mScene->HasMeshes())
        {
            LOG("ERROR: NO MESHES FOUND");
            return false;
        }

        mModelSpaceTransform = mScene->mRootNode->mTransformation;
        mModelSpaceTransform.Inverse();

    
        for(size_t i=0;i<mScene->mNumAnimations;++i)
        {
             LOG(mScene->mAnimations[i]->mName.C_Str());
             std::string animName(mScene->mAnimations[i]->mName.C_Str());
             sk_model_pair_t pair(animName, i);
             mAnimationNameIndices.insert(pair);
        }

        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Bone> bones;
        
        size_t numIndices, numVertices;
        countMeshData(mScene, numVertices, numIndices);
             
        vertices.reserve(numVertices);
        indices.reserve(numIndices);
        bones.resize(numVertices);
        

        sk_nodes_container_t animNodeChecklist;
        sk_model_container_t boneNameIndices;
   
        std::stack<aiNode*> s;
        s.push(mScene->mRootNode);

        while(!s.empty())
        {
            aiNode* n = s.top();
            s.pop();

            for(size_t i=0;i<n->mNumMeshes;++i)
            {
                aiMesh* mesh = mScene->mMeshes[n->mMeshes[i]];

                processMeshBones(mScene, n, mesh, vertices.size(), bones, boneNameIndices, animNodeChecklist);
                Model::mMeshes.push_back(Model::processMesh(mScene, mesh, vertices, indices));
            }
                
            for(size_t i=0;i<n->mNumChildren;++i)
            {
                s.push(n->mChildren[i]);
            }    
        }
        createAnimNode(mScene, mScene->mRootNode, NULL, animNodeChecklist);
        //init buffers
        initBuffers(vertices, indices, bones);
        

        return true;
    }
    

    void createAnimNode(const aiScene* scene, aiNode* node, AnimNode* parent, sk_nodes_container_t& animNodeChecklist)
    {
   //     LOG("AN START: " << node->mName.C_Str());
        
        sk_nodes_iter_t it = animNodeChecklist.find(node);
        if(it != animNodeChecklist.end())
        {

            AnimNode* animNode = new AnimNode(*node, NULL);

            if(parent)
            {
                parent->mChildren.push_back(animNode);
            }
            else 
            {
                mAnimNodeRoot = animNode;
            }
            
            if(it->second != -1)
            {
                animNode->mBoneTransformIndex = it->second;
            } 

           animNode->mAnimTypes.reserve(scene->mNumAnimations);

            for(size_t i=0;i<scene->mNumAnimations;++i)
            {
                aiAnimation* anim = scene->mAnimations[i];
                bool hasAnim = false;
                if(anim->mTicksPerSecond <= 0.0)
                {
                    anim->mTicksPerSecond = AnimNode::DEFAULT_TICKS_PER_SECOND; 
                }
               
                if(anim->mDuration <= 0.0)
                {
                    anim->mDuration = AnimNode::DEFAULT_TICKS_DURATION;
                }

                for(size_t j=0;j<anim->mNumChannels;++j)
                {
                    aiNodeAnim* channel = anim->mChannels[j];
                    if(std::string( channel->mNodeName.C_Str() ) == std::string(node->mName.C_Str() ) )
                    {
             //           LOG("HAS CHANNEL:" << channel->mNodeName.C_Str() );
                        hasAnim = true;
                        animNode->mAnimTypes.push_back(channel);
                        break;
                    }
                }
           
                if(!hasAnim)
                {
                    animNode->mAnimTypes.push_back(NULL);
                }
            } 
            
            for(size_t i=0;i<node->mNumChildren;++i)
            {
                createAnimNode(scene, node->mChildren[i], animNode, animNodeChecklist);
            }
        }
    }

    unsigned int getUpdatedBoneIndex(sk_model_container_t& boneNameIndices, const char* boneName)
    {
        std::string boneNameStr(boneName);
        sk_model_iter_t it = boneNameIndices.find(boneNameStr);                
        // add bone transform at bone index if no name found
        if(it == boneNameIndices.end())
        {
            unsigned int boneIndex = mNumBones++;
            mBoneTransforms.push_back(BoneTransform());
            boneNameIndices[boneNameStr] = boneIndex;
            
            return boneIndex;
        }
        return it->second;
    }

    void processMeshBones(const aiScene* scene, aiNode* node, aiMesh* mesh, size_t baseVertex, std::vector<Bone>& bones,
                          sk_model_container_t& boneNameIndices, sk_nodes_container_t& animNodeChecklist)
    {
        if(mesh->HasBones())
        {
            LOG("IN BONES");

            for(size_t i=0;i<mesh->mNumBones;++i)
            {
                aiBone* bone = mesh->mBones[i];
                const char* boneName = bone->mName.C_Str(); 
                unsigned int boneIndex = getUpdatedBoneIndex(boneNameIndices, boneName);
                mBoneTransforms[boneIndex].mBoneSpaceTransform = mesh->mBones[i]->mOffsetMatrix;

                aiNode* key = scene->mRootNode->FindNode(bone->mName); 
                if(key) //node where name = bone name found
                {
                    animNodeChecklist[key] = boneIndex;
         //           LOG("KEY FOUND: " << key->mName.C_Str() );
                    
                    if(key != node)
                    for(key = key->mParent;key && (key != node || key != node->mParent); key = key->mParent)
                    {   //set tree branch as animatable
                      //  LOG("ADDING TO ANIM " << key->mName.C_Str() );
                        animNodeChecklist.insert( sk_nodes_pair_t(key, -1 ) );

                    }
                }


                for(size_t j=0;j<bone->mNumWeights;++j)
                {
                    aiVertexWeight& vertexWeight = bone->mWeights[j];
                    Bone& boneVertexAttr = bones[baseVertex + vertexWeight.mVertexId];
                    
                    for(size_t k=0;k<BONES_PER_VERTEX;++k)
                    {
                        if(boneVertexAttr.mWeights[k] == 0.0f) 
                        {
                            boneVertexAttr.mBoneIndices[k] = boneIndex;
                            boneVertexAttr.mWeights[k] = vertexWeight.mWeight;
                            break;
                        }
                    } 
                }
            }
        }  
    
    }

    void processAnimNode(float progress, aiMatrix4x4& parentTransform, AnimNode* animNode, unsigned int animationIndex)
    {
       // LOG("ANIM PROCESS"); 
        aiMatrix4x4 animTransform = animNode->getAnimatedTransform(progress, animationIndex);
        aiMatrix4x4 currTransform = parentTransform * animTransform;

        if(animNode->hasRelatedBone())
        {

            //update bone
            BoneTransform& transforms = mBoneTransforms[animNode->mBoneTransformIndex];

            transforms.mAnimatedTransform = mModelSpaceTransform * currTransform * transforms.mBoneSpaceTransform;

            //change row-order, since assimp is cur for dx
            transforms.mAnimatedTransform = transforms.mAnimatedTransform.Transpose();
        }

        for(size_t i=0;i<animNode->mChildren.size();++i)
        {            
            processAnimNode(progress, currTransform, animNode->mChildren[i] , animationIndex);
        }
    }

    void update(float currTime, const std::string& animationName)
    {     
        sk_model_iter_t it = mAnimationNameIndices.find(animationName);
        if(it == mAnimationNameIndices.end())
        {
            LOG("ERROR: WRONG ANIMATION NAME");
            return;
        }
        update(currTime, it->second);

    }

    void update(float currTime, unsigned int animationIndex = 0)
    {
        if(animationIndex >= mScene->mNumAnimations) 
        {
            LOG("ERROR: WRONG ANIMATION INDEX");
            return;
        }
      //  LOG("UDP");
        aiMatrix4x4 initialTransform;
        float progress = currTime * mScene->mAnimations[0]->mTicksPerSecond;
        progress = fmod( progress, mScene->mAnimations[0]->mDuration);
      //  LOG("PROCESS");
        processAnimNode(progress, initialTransform, mAnimNodeRoot, animationIndex);
    }
    
    void initBuffers(std::vector<Vertex>& vertices,  std::vector<GLuint>& indices,  std::vector<Bone>& bones) 
    {

        glGenVertexArrays(1, &mVAO);
       
        glBindVertexArray(mVAO);

        initVertices(vertices);
        initIndices(indices);
        initBones(bones);

        glBindVertexArray(0);
     
    }


    void draw(glm::mat4& viewProj, const Shader& shader)
    {
        glBindVertexArray(mVAO);
        glm::mat4 MVP = viewProj;
        glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), "u_MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
        
        for(size_t i=0;i<mBoneTransforms.size();++i)
        {
            std::string boneStr("u_Bones["+ std::to_string(i) + "]");
            aiMatrix4x4 t = mBoneTransforms[i].mAnimatedTransform;

            glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), boneStr.c_str()), 1, GL_FALSE, &t.a1);
        }


        for(size_t i=0; i<mMeshes.size();++i)
        {
            mMeshes[i].draw();
        }

        glBindVertexArray(0);
    }



private:
    GLuint mBoneBuffer;  

    void initBones(std::vector<Bone>& bones)
    {
        glGenBuffers(1, &mBoneBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, mBoneBuffer);
        glBufferData(GL_ARRAY_BUFFER, bones.size() * sizeof(bones[0]), &bones[0], GL_STATIC_DRAW);  
     
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Bone), (GLvoid*)0);
     
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Bone), (const GLvoid*)offsetof(Bone, mWeights));

    }
    
    void unload()
    {
        Model::unload();
        glDeleteBuffers(1, &mBoneBuffer);
    }
    
    void deleteAnimNodes(AnimNode* node)
    {
        for(size_t i=0;i<node->mChildren.size();++i)
        {
            deleteAnimNodes(node->mChildren[i]);
        }
        delete node;
    }
    
};

