#ifndef ALM_G_BUFFER_H
#define ALM_G_BUFFER_H

#include <vector>
#include <GL/glew.h>
#include "utils.h"


class GBuffer
{
public:
    GBuffer();
    virtual ~GBuffer();
    bool init(size_t w, size_t h); 

    void unload();
    GLuint mBuffer;

private:

    GLuint mPosition, mNormal, mColor, mDepth;

};




class GBufferManager
{
public:
    GBufferManager();
    virtual ~GBufferManager();

  //  GBuffer& getNewGBugger
    GBuffer& getGBuffer(size_t i, GBuffer* ref);
private:
    std::vector<GBuffer> mGBuffers;

};

#endif
