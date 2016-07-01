#ifndef ALM_SKYBOX_H
#define ALM_SKYBOX_H

#include <GL/glew.h>
#include "texture_manager.h"
class Skybox 
{
public:
    Skybox();
    virtual ~Skybox();

    void init();

    void draw();
    void unload();


private:
    GLuint mVAO, mVBO, mEBO;
    GLuint mCubeMapTexture;
};


#endif
