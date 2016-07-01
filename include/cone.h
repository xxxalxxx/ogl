#ifndef ALM_CONE_H
#define ALM_CONE_H

#include <stddef.h>
#include <math.h>
#include <vector>
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "utils.h"
class Cone
{
public:
    Cone();
    virtual ~Cone();

    bool init(size_t numSlices, float height = 1.0f, float bottomCapRadius = 1.0f);
    void initBuffers(const std::vector<glm::vec3>& vertices, const std::vector<GLuint>& indices);

    void draw();
    void unload();

    size_t mNumIndices;

    GLuint mVAO, mVBO, mEBO;
};


#endif
