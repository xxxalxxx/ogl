#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

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



GLint TextureFromFile(const char* path, std::string directory, bool gamma = false);
void getGlTextureFormatFromSOIL(bool* isValidChanned, GLenum* out, unsigned int channel);

class Model 
{
public:
    /*  Model Data */
    std::vector<Texture> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh> meshes;
    std::string directory;
    std::string mTexturesDir;
    bool gammaCorrection;

    /*  Functions   */
    // Constructor, expects a filepath to a 3D model.
    Model(const std::string& path, const std::string& texturesDir, Shader shader, int aiProcessArgs = 0, bool gamma = false) : gammaCorrection(gamma)
    {
        LOG("MODEL_CREATE");
        mTexturesDir = texturesDir;
        this->loadModel(path, aiProcessArgs, shader);
    }

    // Draws the model, and thus all its meshes
    void Draw(glm::mat4& viewProj, Shader shader)
    {
        for(GLuint i = 0; i < this->meshes.size(); i++)
            this->meshes[i].Draw(viewProj, shader);
    }
    
private:

    /*  Functions   */
    // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(const std::string& path, int aiProcessArgs, Shader shader)
    {
        // Read file via ASSIMP
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
        // Retrieve the directory path of the filepath
            LOG("MODEL BEFORE DIR");

        this->directory = path.substr(0, path.find_last_of('/'));
 LOG("MODEL AFTER DIR");


        // Process ASSIMP's root node recursively
        //this->processNode(scene->mRootNode, scene);

        for(size_t i=0;i<scene->mNumMeshes;++i)
        {
            aiMesh* mesh = scene->mMeshes[i]; 
            this->meshes.push_back(this->processMesh(mesh, scene, shader));
        }
    }


    Mesh processMesh(aiMesh* mesh, const aiScene* scene, Shader shader)
    {
 LOG("MODEL PROCESS MESH");


        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;

        for(GLuint i = 0; i < mesh->mNumVertices; i++)
        {

          //  LOG("IN VERTICES");
            Vertex vertex;
            glm::vec3 vector; 
            if(mesh->HasPositions()) 
            {
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;
                vertex.Position = vector;
            }
                      // Normals
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
                // Bitangent
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

            vertices.push_back(vertex);
           
        }
        // Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(GLuint i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // Retrieve all indices of the face and store them in the indices vector
            for(GLuint j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
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
        
        // Return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures, shader);
    }

    // Checks all material textures of a given type and loads the textures if they're not loaded yet.
    // The required info is returned as a Texture struct.
    void loadMaterialTextures(std::vector<Texture>& textures, aiMaterial* mat, aiTextureType type)
    {
        for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            GLboolean skip = false;
            for(GLuint j = 0; j < textures_loaded.size(); j++)
            {
                if(textures_loaded[j].path == str)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }

            if(!skip)
            {   // If texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), mTexturesDir);
                texture.path = str;
                texture.type = type;
                textures.push_back(texture);
                this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
       
    }
};

void getGlTextureFormatFromSOIL(bool* isValidChannel, GLenum* out, unsigned int channel)
{
    //param channels the number of channels: 1-luminous, 2-luminous/alpha, 3-RGB, 4-RGBA
    *isValidChannel = true;
    switch(channel)
    {
        case 1: *out = GL_RED;  break;
        case 2: *out = GL_RG;   break;
        case 3: *out = GL_RGB;  break;
        case 4: *out = GL_RGBA; break;
        default: *isValidChannel = false;
    }
}


GLint TextureFromFile(const char* path, std::string directory, bool gamma)
{
     //Generate texture ID and load texture data 
    LOG("LOADING TEX");
    std::string filename(path);
    if(directory[directory.size() -1] != '/') directory += '/';
    filename = directory + filename;
    LOG(filename.c_str());
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    int w, h;
    int channels;

    LOG("BEFORE LOAD");
    unsigned char* image = SOIL_load_image(filename.c_str(), &w, &h, &channels, SOIL_LOAD_AUTO);
    
    LOG("AFTER LOAD");
    GLenum format;
    bool validChannel;
    getGlTextureFormatFromSOIL(&validChannel, &format, channels);
    LOG(channels);
    if(!validChannel) format = gamma ? GL_SRGB : GL_RGB; 

    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    LOG("AFTER BIND");
    glTexImage2D(GL_TEXTURE_2D, 0, format,  w, h, 0, format, GL_UNSIGNED_BYTE, image);
    LOG("AFTER TEX IMAGE");
    glGenerateMipmap(GL_TEXTURE_2D);	

    LOG("AFTER MIP");
    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    LOG("AFTER TEX");
    return textureID;
}
