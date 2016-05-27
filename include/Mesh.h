#ifndef ALM_MESH_H
#define ALM_MESH_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>


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

    Mesh();
    Mesh(
        unsigned int numIndices,
        unsigned int materialIndex,
        unsigned int baseVertex,
        unsigned int baseIndex);

    void draw(std::vector<std::vector<Texture> >& materials, ModelTechnique& tech);

    unsigned int mNumIndices, 
                 mMaterialIndex, 
                 mBaseVertex, 
                 mBaseIndex;
};

#endif

