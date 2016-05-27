#ifndef ALM_MODEL_H
#define ALM_MODEL_H

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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "TextureManager.h"
#include "FileSystem.h"
#include "ModelTechnique.h"

class Model 
{
public:
    Model(const std::string& fileName, 
          const std::string& texturesDir,          
          bool replaceTexturePath = false,  
          int aiProcessArgs = 0, 
          bool gamma = false
         );

    Model(
        const std::string& fileName, 
        const char* texturesDir = NULL,          
        bool replaceTexturePath = false,  
        int aiProcessArgs = 0, 
        bool gamma = false
        );

    virtual ~Model();

    bool init();
    void draw(ModelTechnique& tech);    
    void unload();


    std::vector<Mesh> mMeshes;

    std::string mFileName;
    std::string mTexturesDir;

    std::vector<std::vector<Texture> > mMaterials;
  
protected:
    Mesh processMesh(const aiScene* scene, aiMesh* mesh, 
             std::vector<Vertex>& vertices, std::vector<GLuint>& indices);
 
    void processNode(const aiScene* scene, aiNode* node,
            std::vector<Vertex>& vertices, std::vector<GLuint>& indices);

    void loadMaterialTextures(std::vector<Texture>& textures, aiMaterial* mat, aiTextureType type);

    void initBuffers(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
    void initVertices(const std::vector<Vertex>& vertices);
    void initIndices(const std::vector<GLuint>& indices);

    void countMeshData(const aiScene* scene, size_t& outNumVertices, size_t& outNumIndices);

  
    int mAiProcessArgs;
   
    bool gammaCorrection;
    bool mReplaceTexturePath;

    GLuint mVAO, mVBO, mEBO;

private:
    void setVector(const aiVector3D& src, glm::vec3& dest);
};


#endif
