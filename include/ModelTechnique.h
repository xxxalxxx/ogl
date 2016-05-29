#ifndef ALM_MESH_TECHNIQUE_H
#define ALM_MESH_TECHNIQUE_H

#include <vector>
#include <string>

#include <assimp/scene.h>

#include "Utils.h"
#include "Technique.h"

#define NUM_TEXTURE_TYPES 13

struct Texture {
    GLuint id;
    aiTextureType type;
    GLuint samplerHandle;
};


class ModelTechnique : public Technique
{
public:
    ModelTechnique(const Shader& shader);
    
    ModelTechnique(const Shader& shader,
                   const std::string& vertexShaderPath,
                   const std::string& fragmentShaderPath,
                   bool makeAbsPaths = true);

    ModelTechnique(const std::string& vertexShaderPath,
                   const std::string& fragmentShaderPath,
                   bool makeAbsPaths = true);

    virtual ~ModelTechnique();
    ModelTechnique& setUniformSampler(size_t materialIndex, GLuint materialHandlesIndex);
    ModelTechnique& setHandleMaterials(std::vector<std::vector<Texture> >& materials);
protected:
    void addMaterialTextures(const std::vector<Texture>& textures);

    static const char * const mTextureTypeNames[];

    std::vector<std::vector<GLuint > > mMaterialHandles;
};


#endif
