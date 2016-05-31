#ifndef ALM_G_BUFFER_H
#define ALM_G_BUFFER_H

#include <vector>

#include "utils.h"


class GBuffer
{
public:
    GBuffer();
    virtual ~GBuffer();
    void init(size_t w, size_t h); 
    void update(size_t w, size_t h);
private:

    GLuint mBufferHandle;

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
