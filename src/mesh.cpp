#include "mesh.h"

Mesh::Mesh() : mNumIndices(0), mMaterialIndex(0), mBaseVertex(0), mBaseIndex(0)
{
}

Mesh::Mesh(unsigned int numIndices, unsigned int materialIndex, unsigned int baseVertex, unsigned int baseIndex) : 
    mNumIndices(numIndices), mMaterialIndex(materialIndex), mBaseVertex(baseVertex), mBaseIndex(baseIndex)
{
}

void Mesh::draw(std::vector<std::vector<Texture> >& materials, ModelTechnique& tech) 
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

