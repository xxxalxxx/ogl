#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

#include "glm/gtc/type_ptr.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Shader.h"

#define LOG(x) std::cout<< x << std::endl

struct Vertex {
    glm::vec3 Position, Normal, Tangent, Bitangent;
    glm::vec2 TexCoords;
};

struct Texture {
    GLuint id;
    aiTextureType type;
    GLuint samplerHandle;
};

class Mesh {
public:

    std::vector<Texture> mTextures;

    struct Uniforms {GLuint program, MVP;} mUniforms;

    Mesh(){}

    Mesh(
        std::vector<Texture>& textures, 
        const Shader& shader
        ) : mNumIndices(0), mMaterialIndex(0), mBaseVertex(0), mBaseIndex(0)
    {
        LOG("MESH INIT");
        initUniforms(shader, textures);
    }



    // Render the mesh
    void Draw(glm::mat4& viewProj, const Shader& shader) 
    {
        for(GLuint i = 0; i < mTextures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
            
            glUniform1i(mTextures[i].samplerHandle, i);
            glBindTexture(GL_TEXTURE_2D, mTextures[i].id);
        }
      
        glDrawElementsBaseVertex(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, 
        (void*)(sizeof(unsigned int) * mBaseIndex), mBaseVertex);


        for (GLuint i = 0; i < mTextures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void initUniforms(const Shader& shader, std::vector<Texture>& textures)
    {
        mUniforms.program = shader.getProgram();
        mUniforms.MVP = glGetUniformLocation(mUniforms.program, "u_MVP");
        
        int indexDiffuse = 0, indexNormal = 0, indexSpec = 0, indexHeight = 0;
        mTextures = textures;

        for(size_t i=0;i<textures.size();++i)
        {
            int indexCurr = -1;
            std::string name;
            switch(textures[i].type)
            {
                case aiTextureType_DIFFUSE:
                    name = "texture_diffuse";
                    indexCurr = ++indexDiffuse;
                    break;
                case aiTextureType_NORMALS:
                    name = "texture_normal";
                    indexCurr = ++indexNormal;
                    break;
                case aiTextureType_SPECULAR:
                    name = "texture_specular";
                    indexCurr = ++indexSpec;
                    break;
                case aiTextureType_HEIGHT:
                    name = "texture_height";
                    indexCurr = ++indexHeight;
                    break;
                default:break;
            };


            if(indexCurr != -1) 
            {
                name += std::to_string(indexCurr);
             
                mTextures[i].samplerHandle = glGetUniformLocation(mUniforms.program, name.c_str());
            }


            std::cout << "TEX NAME:" << name << std::endl;
        }

    }

    

    unsigned int mNumIndices, 
                 mMaterialIndex, 
                 mBaseVertex, 
                 mBaseIndex;


private:


};



