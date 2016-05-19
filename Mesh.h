#ifndef ALM_MESH_H
#define ALM_MESH_H

#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <stddef.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"


struct Vertex 
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

struct Texture
{
    GLuint textureHandle, samplerHandle;
    aiTextureType textureType;
};

class Mesh
{
public:
    Mesh(GLuint program, 
              const std::vector<Vertex>& vertices,
              const std::vector<unsigned int>& indices,
                    std::vector<Texture>& textures);
    ~Mesh();
   // void update(const glm::mat4& model, float dt);
    void draw(const glm::mat4& model, const glm::mat4& viewProj);
    void unload();

private:
    void initBuffers(const std::vector<Vertex>& vertices, 
                     const std::vector<unsigned int>& indices);
    void initUniforms();

    GLuint mVBO, mVAO, mEBO;
    
    struct Uniforms
    {
        Uniforms(GLuint program, std::vector<Texture>& textures);
        GLuint program, MVP; 
        std::vector<Texture>& textures;  
    } mUniforms;
  
    size_t mNumIndices;
};


#endif
