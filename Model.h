#ifndef ALM_MODEL_H
#define ALM_MODEL_H

#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "Mesh.h"

#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "SOIL/SOIL.h"

class Model
{
public:
    Model();
    ~Model();

    bool init(GLuint program, const std::string& filePath);
    void update(const glm::mat4& model, float dt);
    void draw(const glm::mat4& viewProj);

private:

    
    void addTextures(std::vector<Texture>& in, aiMaterial* material, aiTextureType textureType, const std::string& fileName);
    std::string getFolderFromFileName(const std::string& filePath);

    GLuint getTextureHandle(const std::string& filePath);
    std::vector<Mesh> mMeshes;

    static std::unordered_map<std::string, Texture> mModelTextures;
    glm::mat4 mModelMatrix;
};


#endif
