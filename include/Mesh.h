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
#include "Utils.h"
#include "Shader.h"
#include "ModelTechnique.h"


struct Vertex {
    glm::vec3 Position, Normal, Tangent, Bitangent;
    glm::vec2 TexCoords;

};
class Mesh {
public:

    Mesh() : mNumIndices(0), mMaterialIndex(0), mBaseVertex(0), mBaseIndex(0)
    {
    }

    Mesh(
        unsigned int numIndices,
        unsigned int materialIndex,
        unsigned int baseVertex,
        unsigned int baseIndex) : mNumIndices(numIndices), mMaterialIndex(materialIndex), mBaseVertex(baseVertex), mBaseIndex(baseIndex)
    {
    }


    // Render the mesh
    void draw(std::vector<std::vector<Texture> >& materials, ModelTechnique& tech) 
    {
        std::vector<Texture>& textures = materials[mMaterialIndex];

        for(GLuint i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);            
            tech.setUniformSampler(mMaterialIndex, i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
      
        glDrawElementsBaseVertex(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, 
        (void*)(sizeof(unsigned int) * mBaseIndex), mBaseVertex);


        for (GLuint i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    
    unsigned int mNumIndices, 
                 mMaterialIndex, 
                 mBaseVertex, 
                 mBaseIndex;
};



