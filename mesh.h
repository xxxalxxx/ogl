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
    aiString path;
    GLuint samplerHandle;
};

class Mesh {
public:

    std::vector<Texture> mTextures;

    struct Uniforms {GLuint program, MVP;} mUniforms;



    /*  Functions  */
    // Constructor
    Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures, Shader shader)
    {
        // Now that we have all the required data, set the vertex buffers and its attribute pointers.
        //this->setupMesh()
        LOG("MESH INIT");
        initBuffers(vertices, indices);
        initUniforms(shader, textures);
    }

    // Render the mesh
    void Draw(glm::mat4& viewProj, Shader shader) 
    {
        glm::mat4 MVP = viewProj;
        glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), "u_MVP"), 1, GL_FALSE, glm::value_ptr(MVP));

        for(GLuint i = 0; i < mTextures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
            
            glUniform1i(mTextures[i].samplerHandle, i);
            glBindTexture(GL_TEXTURE_2D, mTextures[i].id);
        }
        
        // Draw mesh
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Always good practice to set everything back to defaults once configured.
        for (GLuint i = 0; i < mTextures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void initUniforms(Shader shader, std::vector<Texture>& textures)
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


    void initBuffers(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
    {
        mNumIndices = indices.size();
        // Create buffers/arrays
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        glGenBuffers(1, &mEBO);

        glBindVertexArray(mVAO);
        // Load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumIndices * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        // Set the vertex attribute pointers
        // Vertex Positions
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
        glDeleteVertexArrays(1, &mVAO);
        glDeleteBuffers(1, &mVBO);
        glDeleteBuffers(1, &mEBO);
    }

private:
    int mNumIndices;
    GLuint mVAO, mVBO, mEBO;

  };



