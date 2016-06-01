#ifndef ALM_SPHERE_H
#define ALM_SPHERE_H

#include <math.h>

#include <vector>

#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat4x4.hpp"

class Sphere
{
public:
    Sphere();
    virtual ~Sphere();
    bool init(size_t numStacks, size_t numSlices, float radius = 1.0f);
    void draw();
    void initBuffers(const std::vector<glm::vec3>& vertices, const std::vector<GLuint>& indices);

private:
    GLuint mVAO, mEBO, mVBO;
    size_t mNumIndices;
};


#endif
