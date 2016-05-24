#include "model.h"
#include "skinned_mesh.h"
#include <map>
#include <stack>
#include <math.h>
#include "glm/gtx/string_cast.hpp"


class SkinnedModel : public Model 
{
public:

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
    sk_model_container_t mBoneNameIndices;

    std::vector<BoneTransform> mBoneTransforms;
    aiMatrix4x4 mModelSpaceTransform;

    size_t mNumBones;

    SkinnedModel(
            const char* path, 
            const char* texturesDir,
            Shader& shader,   
            int aiProcessArgs = 0, 
            bool gamma = false
            ): Model(path, shader, texturesDir, aiProcessArgs, gamma), mScene(NULL), mNumBones(0)
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

        mModelSpaceTransform = mScene->mRootNode->mTransformation;
        mModelSpaceTransform.Inverse();

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
             
        LOG("NV:" << numVertices);
        vertices.reserve(numVertices);
        indices.reserve(numIndices);
        bones.resize(numVertices);
        

        sk_nodes_container_t animNodeChecklist;
                
        std::stack<aiNode*> s;
        s.push(mScene->mRootNode);

        while(!s.empty())
        {
            aiNode* n = s.top();
            s.pop();

            for(size_t i=0;i<n->mNumMeshes;++i)
            {
                aiMesh* mesh = mScene->mMeshes[n->mMeshes[i]];

                processMeshBones(mScene, n, mesh, vertices.size(), bones, animNodeChecklist);
                mMeshes.push_back(processMesh(mScene, mesh, vertices, indices));
            }
                
            for(size_t i=0;i<n->mNumChildren;++i)
            {
                s.push(n->mChildren[i]);
            }    
        }
        //init buffers
        LOG("BUF INIT");
        initBuffers(vertices, indices, bones);
        LOG("BEFORE ANIM"); 
        createAnimNode(mScene, mScene->mRootNode, NULL, animNodeChecklist);
        LOG("AFTER ANIM");
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
                parent->mChildren.push_back(animNode);
            else 
                mAnimNodeRoot = animNode;

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
                    anim->mTicksPerSecond = 25.0f; 

               // if(anim->mDuration <= 0.0)
               // {
                 //   anim->mDuration = DEFAULT_ANIM_DURATION;
               // }

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
                    animNode->mAnimTypes.push_back(NULL);
            } 
            
            for(size_t i=0;i<node->mNumChildren;++i)
            {
                createAnimNode(scene, node->mChildren[i], animNode, animNodeChecklist);
            }
        }
    }

    unsigned int getUpdatedBoneIndex(const char* boneName)
    {
        std::string boneNameStr(boneName);
        sk_model_iter_t it = mBoneNameIndices.find(boneNameStr);                
        // add bone transform at bone index if no name found
        if(it == mBoneNameIndices.end())
        {
            unsigned int boneIndex = mNumBones++;
            mBoneTransforms.push_back(BoneTransform());
            mBoneNameIndices[boneNameStr] = boneIndex;
            
            return boneIndex;
        }
        return it->second;
    }

    void processMeshBones(const aiScene* scene, aiNode* node, aiMesh* mesh, size_t baseVertex, std::vector<Bone>& bones, sk_nodes_container_t& animNodeChecklist)
    {
        if(mesh->HasBones())
        {
            LOG("IN BONES");

            for(size_t i=0;i<mesh->mNumBones;++i)
            {
                aiBone* bone = mesh->mBones[i];
                const char* boneName = bone->mName.C_Str(); 
                unsigned int boneIndex = getUpdatedBoneIndex(boneName);
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

                LOG("AFTER BONES");
                for(size_t j=0;j<bone->mNumWeights;++j)
                { LOG("IN W");
                    aiVertexWeight& vertexWeight = bone->mWeights[j];
                    Bone& boneVertexAttr = bones[baseVertex + vertexWeight.mVertexId];
                    LOG("A W");
                    for(size_t k=0;k<BONES_PER_VERTEX;++k)
                    {
                        if(boneVertexAttr.mWeights[k] == 0.0f) 
                        {       LOG("W FOUND");
                            boneVertexAttr.mBoneIndices[k] = boneIndex;
                            boneVertexAttr.mWeights[k] = vertexWeight.mWeight;
                            break;
                        }
                    } 
                }
            }
        }  
    LOG("BC END");
    }

    void processAnimNode(float progress, aiMatrix4x4& parentTransform, AnimNode* animNode, unsigned int animationIndex)
    {
       // LOG("ANIM PROCESS"); 
        aiMatrix4x4 animTransform = animNode->getAnimatedTransform(progress, animationIndex);
        aiMatrix4x4 currTransform = parentTransform * animTransform;

        if(animNode->hasRelatedBone())
        {
       //     LOG("IN UPD BONE:" << animNode->mBoneTransformIndex);
            //update bone
            BoneTransform& transforms = mBoneTransforms[animNode->mBoneTransformIndex];
            transforms.mAnimatedTransform = mModelSpaceTransform * currTransform * transforms.mBoneSpaceTransform;
            
            //change row-order
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
        aiMatrix4x4 initialTransform;
        float progress = currTime * mScene->mAnimations[0]->mTicksPerSecond;
        progress = fmod( progress, mScene->mAnimations[0]->mDuration);
        processAnimNode(progress, initialTransform, mAnimNodeRoot, animationIndex);
    }
    
    void initBuffers(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<Bone>& bones) 
    {
   /*     for(size_t i=0;i<bones.size();++i)
        {
            LOG( glm::to_string(bones[i].mBoneIndices) << " " << glm::to_string(bones[i].mWeights));
        }
*/
        glGenVertexArrays(1, &mVAO);
 
        glGenBuffers(1, &mVBO);
        glGenBuffers(1, &mEBO);
        glGenBuffers(1, &mBoneBuffer);

        glBindVertexArray(mVAO); 

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  
    
        //Vertex Positions
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
        // Vertex Normals
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
        // Vertex Texture Coords
        glEnableVertexAttribArray(2);	
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

       // glEnableVertexAttribArray(3);
      //  glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, bi));
      
      //  glEnableVertexAttribArray(3);
      //  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*) offsetof(Vertex, bi));

      //  glEnableVertexAttribArray(4);
      //  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*) offsetof(Vertex, w));
     /* 
        glBindBuffer(GL_ARRAY_BUFFER, mBoneBuffer);
        glBufferData(GL_ARRAY_BUFFER, bones.size() * sizeof(bones[0]), &bones[0], GL_STATIC_DRAW);  
     
        glEnableVertexAttribArray(3);
        glVertexAttribIPointer(3, 4, GL_INT, sizeof(Bone), (GLvoid*)0);
     
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Bone), (const GLvoid*) (16));
     */
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
    
        glBindVertexArray(0);

        LOG("END");
     
    }

    void glLog(const char* msg)
    {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << msg << " OpenGL error: " << err <<std::endl;
             
        } 

    }

    void Draw(glm::mat4& viewProj, const Shader& shader)
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
            mMeshes[i].Draw();
        }

        glBindVertexArray(0);
    }



private:
    GLuint mBoneBuffer;  

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

