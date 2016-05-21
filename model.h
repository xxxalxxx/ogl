#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <stdexcept>


// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include "glm/glm.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

#include <SOIL/SOIL.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "TextureManager.h"

class Model 
{
public:
  
   // std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;

    std::string mPath;
    std::string mTexturesDir;
  
    Shader& mShader;
    int mAiProcessArgs;
   
    bool gammaCorrection;
    
    size_t mNumVertices, 
           mNumIndices;


    std::vector<Vertex> mVertices;
    std::vector<GLuint> mIndices;
    std::vector<Texture> mTextures;

   GLuint mVAO, mVBO, mEBO;

    Model(
        const char* path, 
        const char* texturesDir, 
        Shader& shader, 
        int aiProcessArgs = 0, 
        bool gamma = false
        ) : mPath(path), mTexturesDir(texturesDir), mShader(shader), mAiProcessArgs(aiProcessArgs), gammaCorrection(gamma), 
            mNumVertices(0), mNumIndices(0)
    {
        LOG("MODEL_CREATE");  
    }

    void init()
    {
         this->loadModel(mPath, mAiProcessArgs, mShader);
         initBuffers(mVertices, mIndices);
    }

    void Draw(glm::mat4& viewProj, const Shader& shader)
    {
       glm::mat4 MVP = viewProj;
        glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), "u_MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
        glBindVertexArray(mVAO);

        for(GLuint i = 0; i < this->meshes.size(); i++)
            this->meshes[i].Draw(viewProj, shader);

        glBindVertexArray(0);
    }


    Mesh processMesh(const aiScene* scene, aiMesh* mesh,  const Shader& shader)
    {
        LOG("MODEL PROCESS MESH");

        std::vector<Texture> textures;

        mVertices.reserve(mesh->mNumVertices);

        for(GLuint i = 0; i < mesh->mNumVertices; i++)
        {

            Vertex vertex;
            glm::vec3 vector; 
            if(mesh->HasPositions()) 
            {
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;
                vertex.Position = vector;
            }

            if(mesh->HasNormals()) 
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }

            if(mesh->HasTangentsAndBitangents()) 
            {
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;

                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }  

            if(mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else 
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }
            mVertices.push_back(vertex);
           
        }

        //process indices
        
        mIndices.reserve(mesh->mNumFaces * 3);
        size_t numIndices = mIndices.size(); 
        
        for(GLuint i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace& face = mesh->mFaces[i];

            for(GLuint j = 0; j < face.mNumIndices; j++)
            {
                mIndices.push_back(face.mIndices[j]);
            }
           
        }
        numIndices = mIndices.size() - numIndices;
        // Process materials
        if(mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            aiTextureType texTypes[] = {
                aiTextureType_DIFFUSE,
                aiTextureType_SPECULAR,
                aiTextureType_HEIGHT,
                aiTextureType_AMBIENT
            };

            size_t n = sizeof(texTypes)/sizeof(texTypes[0]);
            for(size_t i=0;i<n;++i)
            {
                loadMaterialTextures(textures, material, texTypes[i]);
            }
        }
           
        Mesh ret(textures, shader);
        ret.mNumIndices = numIndices;
        ret.mBaseVertex = mNumVertices;
        ret.mBaseIndex = mNumIndices;

        mNumVertices += mesh->mNumVertices;
        mNumIndices  += numIndices;
        
        return ret;
    }

    
private:
    
    void loadModel(const std::string& path, int aiProcessArgs, const Shader& shader)
    {
        LOG("MODEL LOAD");
        
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, 
                aiProcessArgs ? aiProcessArgs :
                  aiProcess_Triangulate 
                | aiProcess_FlipUVs 
                | aiProcess_CalcTangentSpace
                );
        // Check for errors
        if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }


        if(scene->HasMeshes())
        {
            processNode(scene, scene->mRootNode, shader); 
        }

    }


    void processNode(const aiScene* scene, aiNode* node, const Shader& shader)
    {
        for(size_t i=0;i<node->mNumMeshes;++i)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; 
            this->meshes.push_back(this->processMesh(scene, mesh, shader));
        }
    

        for(size_t i=0;i<node->mNumChildren;++i)
        {
            processNode(scene, node->mChildren[i], shader);
        }
    }


    void loadMaterialTextures(std::vector<Texture>& textures, aiMaterial* mat, aiTextureType type)
    {
        TextureManager& textureManager = TextureManager::getInstance();

        for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString relativeTexFilePath;
            mat->GetTexture(type, i, &relativeTexFilePath);
         
            Texture texture;
            texture.id = textureManager.load(mTexturesDir.c_str(), relativeTexFilePath.C_Str() ); 
            texture.type = type;
            textures.push_back(texture);
        }
       
    }




    void initBuffers(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
    {
        mNumIndices = indices.size();

        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        glGenBuffers(1, &mEBO);

        glBindVertexArray(mVAO);
        // Load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumIndices * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);


        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
        // Vertex Normals
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
        // Vertex Texture Coords
        glEnableVertexAttribArray(2);	
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
        // Vertex Tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));
        // Vertex Bitangent
       // glEnableVertexAttribArray(4);
       // glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0);

    }


    void unload()
    {

        //TODO: clear textures too
        glDeleteVertexArrays(1, &mVAO);
        glDeleteBuffers(1, &mVBO);
        glDeleteBuffers(1, &mEBO);
    }



};

