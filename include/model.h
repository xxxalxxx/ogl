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

#include "mesh.h"
#include "texture_manager.h"
#include "file_system.h"
#include "model_technique.h"

#define ModelFlag_USE_GAMMA 1
#define ModelFlag_STRIP_TEXTURE_PATH 2
#define ModelFlag_USE_ABS_PATH 4

#define DEFAULT_AIPROCESS_ARGS (aiProcess_Triangulate | \
                                aiProcess_GenSmoothNormals | \
                                aiProcess_ImproveCacheLocality | \
                                aiProcess_JoinIdenticalVertices | \
                                aiProcess_SortByPType | \
                                aiProcess_FindDegenerates | \
                                aiProcess_FindInvalidData | \
                                aiProcess_FlipUVs | \
                                aiProcess_CalcTangentSpace | \
                                aiProcess_OptimizeMeshes | \
                                aiProcess_OptimizeGraph)

class Model 
{
public:
    Model(const std::string& fileName, 
          const std::string& texturesDir,
          unsigned int modelFlags = 0,
          int aiProcessArgs = DEFAULT_AIPROCESS_ARGS);

    Model(
        const std::string& fileName, 
        const char* texturesDir = NULL,          
        unsigned int modelFlags = 0,
        int aiProcessArgs = DEFAULT_AIPROCESS_ARGS);

    virtual ~Model();

    bool init();
    void draw(ModelTechnique& tech);    
    void unload();


    std::vector<Mesh> mMeshes;

    std::string mFileName;
    std::string mTexturesDir;

    std::vector<std::vector<Texture> > mMaterials;
     
    int mAiProcessArgs; 
    unsigned int mModelFlags;

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

    GLuint mVAO, mVBO, mEBO;

private:
    void setVector(const aiVector3D& src, glm::vec3& dest);
};


#endif
