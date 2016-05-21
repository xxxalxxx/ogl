#include "Mesh.h"


Mesh::Uniforms::Uniforms(GLuint programRef, std::vector<Texture>& texturesRef): program(programRef), textures(texturesRef)
{

    
}

Mesh::Mesh(GLuint program, 
            const std::vector<Vertex>& vertices,
            const std::vector<unsigned int>& indices,
                  std::vector<Texture>& textures) : mUniforms(program, textures)
{
    initBuffers(vertices, indices);
    initUniforms();   
}

Mesh::~Mesh()
{
    unload();
}

void Mesh::initBuffers(const std::vector<Vertex>& vertices,const std::vector<unsigned int>& indices)
{
    mNumIndices = indices.size();
    std::cout << "in buff " << vertices.size() << " " << indices.size() << std::endl; 
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);
  
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), 
                 &vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), 
                 &indices[0], GL_STATIC_DRAW);

    // Vertex Positions
    glEnableVertexAttribArray(0);   
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                         (GLvoid*)0);
    // Vertex Normals
    glEnableVertexAttribArray(1);   
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                         (GLvoid*)offsetof(Vertex, normal));
    // Vertex Texture Coords
    glEnableVertexAttribArray(2);   
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                         (GLvoid*)offsetof(Vertex, texCoord));

    glBindVertexArray(0);
}

void Mesh::initUniforms()
{
    mUniforms.MVP = glGetUniformLocation(mUniforms.program, "u_MVP");
    std::cout << "u tex sz " << mUniforms.textures.size() << std::endl;
    size_t j = 0;
    for(size_t i=0;i<mUniforms.textures.size();++i)
    {
       Texture& texture = mUniforms.textures[i];
       std::string uniformTextureName = "u_Sampler";
      
       switch(texture.textureType)
       {
            case aiTextureType_DIFFUSE:
                uniformTextureName.append(std::to_string(++j));
                std::cout << "sampler name " << uniformTextureName << std::endl;
                texture.samplerHandle = glGetUniformLocation(mUniforms.program, uniformTextureName.c_str()); 
                break;
            default:
               break; 
       }
    }
    
}

void Mesh::draw(const glm::mat4& model, const glm::mat4& viewProj)
{
  
    glm::mat4 MVP = viewProj;// * model;
    glUniformMatrix4fv(mUniforms.MVP, 1, GL_FALSE, glm::value_ptr(MVP) );

      std::cout<<"after mvp " << mUniforms.textures.size()<< " " << mVAO << " " << mEBO << " " << mVBO << std::endl;

    for(size_t i=0;i<mUniforms.textures.size();++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glUniform1i(mUniforms.textures[i].samplerHandle, i);
        glBindTexture(GL_TEXTURE_2D, mUniforms.textures[i].textureHandle);
    }


    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, 0);
   GLenum e = GL_NO_ERROR;
   while((e=glGetError())!=GL_NO_ERROR){
        std::cout << e << std::endl;
   } 


    glBindVertexArray(0);
}

void Mesh::unload()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
    glDeleteBuffers(1, &mEBO);
}