#ifndef ALM_QUAD_H
#define ALM_QUAD_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "SOIL/SOIL.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat4x4.hpp"

class Quad
{
public:
    Quad();
    ~Quad();
    void init(GLuint program, const char* texturePath);
      
    void update(float dt);
    void draw(const glm::mat4& viewProj, float dt);

    void unload();
private:

    void initBuffers();
    void initTextures(const char* texturePath);
    void initUniforms(GLuint program);

    GLuint mVBO, mVAO, mEBO;

    glm::mat4 mModel;

    struct Uniforms {GLuint program, MVP, texture, sampler; } Uniforms;
    


};


#endif