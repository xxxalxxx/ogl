#ifndef ALM_G_BUFFER_H
#define ALM_G_BUFFER_H

#include <vector>
#include <GL/glew.h>
#include "utils.h"
#include "ssao.h"

class GBuffer
{
public:
    GBuffer();
    virtual ~GBuffer();
    
    bool init(size_t w, size_t h); 

    void setSSAO(SSAO& ssao);

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
    GLuint mPosition, mNormal, mColor, mDepth, mResult;
private:
    void enableAdditiveBlending();
    void enableTextures();

    SSAO* mSSAO;


};


#endif
