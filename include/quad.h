#ifndef ALM_QUAD_H
#define ALM_QUAD_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat4x4.hpp"

#include "technique.h"
#include "texture_manager.h"

class Quad
{
public:
    Quad();
    ~Quad();

    void init(const char* texturePath);
      
    void update(float dt);
    void draw(Technique& Technique);

    void unload();
private:

    void initBuffers();
    void initTextures(const char* texturePath);
    
    GLuint mTexture;
    GLuint mVBO, mVAO, mEBO;
};


#endif
