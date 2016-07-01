#ifndef ALM_HEIGHT_MAP_H
#define ALM_HEIGHT_MAP_H

#include <string>
#include <vector>

#include <GL/glew.h>
#include "glm/common.hpp"
#include "glm/gtx/projection.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtx/string_cast.hpp"

#include "utils.h"
#include "file_system.h"
#include "texture_manager.h"

#include "quad.h"


#define HeightMapFlag_USE_ABS_PATH 1
#define HeightMapFlag_GEN_NORMALS 2
#define HeightMapFlag_GEN_TANGENTS_AND_BINORMALS 4

class HeightMap
{
public:

    struct Layer
    {
        std::string path;
        float step;

        Layer(
                const std::string& path, 
                float step
                ): path(path), step(step)
        {
            Utils::clamp(this->step, 0.0f, 1.0f);
        }

    };

    struct BlendedTexture
    {
        std::string path, blendPath;
        BlendedTexture(
                const std::string& path, 
                const std::string& blendPath
                ): path(path), blendPath(blendPath)
        {
        }
    };

    HeightMap();
    virtual ~HeightMap();
    void init(
            const std::string& heightmapPath, 
            const std::vector<Layer>& layerInfos,
            const std::vector<BlendedTexture>* blendedTexInfos,
            unsigned int flags = HeightMapFlag_GEN_NORMALS | HeightMapFlag_USE_ABS_PATH
            );
    void initBuffers(
            const std::string& heightMapPath, 
            bool useAbsPath,
            bool genNormals = true,
            bool genTangentsAndBinormals = false);
  
    void initVertices(std::vector<glm::vec3>& vertices, const std::string& heightMapPath);
    void initIndices(std::vector<GLuint>& indices);
    void initNormals(std::vector<glm::vec3>& normals, const std::vector<glm::vec3>& vertices);
    void initTangensAndBinormals(
            std::vector<glm::vec3>& tangents, 
            std::vector<glm::vec3>& binormals, 
            const std::vector<glm::vec3>& vertices,
            const std::vector<glm::vec3>& normals);

    void initLayers(const std::string& heightMapPath, const std::vector<Layer>& layers, bool useAbsPath);
    void initBlendedTextures(const std::vector<BlendedTexture>& blendedTextures, bool useAbsPath);

    void draw();
    void unload();

    GLuint mTexture;

private:

    GLuint mVBO, mVAO, mEBO;
    size_t mNumIndices;

    size_t mWidth, mHeight;

    const static size_t MAX_LAYERS;
};



#endif
