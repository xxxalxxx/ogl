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

    void startGeometryPass1();
    void endGeometryPass1();
    
    void startStencilPass();
    void endStencilPass();

    void startLightPass();
    void endLightPass();

    void startGeometryPass2();
    void endGeometryPass2(size_t w, size_t h);

    void unload();
    GLuint mBuffer;

private:
    void enableAdditiveBlending();
    void enableTextures();

    GLuint mPosition, mNormal, mColor, mDiffuse, mSpecular, mDepth;

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
