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
    void init();
    void initBuffers();
    void initShaders();
    void initTextures();
    
    void update(float dt);
    void draw(const glm::mat4& viewProj, GLuint program, float dt);

    void unload();
private:
    GLuint mVBO, mVAO, mEBO;
    GLuint* mProgram;
    GLuint mTexture;

    glm::mat4 mModel;


};


#endif
