#include "g_buffer.h"


GBuffer::GBuffer() : mBuffer(0), mPosition(0), mNormal(0), mDepth(0), mSSAO(NULL)
{

}

GBuffer::~GBuffer()
{
    
}

bool GBuffer::init(size_t w, size_t h)
{
    if(!w || !h) return false;

    glGenFramebuffers(1, &mBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mBuffer);

    glGenTextures(1, &mNormal);
    glBindTexture(GL_TEXTURE_2D, mNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, w, h, 0, GL_RG, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mNormal, 0);
    
    glGenTextures(1, &mColor);
    glBindTexture(GL_TEXTURE_2D, mColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mColor, 0);

    glGenTextures(1, &mResult);
    glBindTexture(GL_TEXTURE_2D, mResult);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mResult, 0);

    glGenTextures(1, &mDepth);
    glBindTexture(GL_TEXTURE_2D, mDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepth, 0);


/*
    glGenRenderbuffers(1, &mDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, mDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepth);
*/
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG("ERROR:: FRAMEBUFFER NOT COMPLETE " << status);
        return false;
    }
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return true;
}

void GBuffer::startGeometryPass1()
{
   glBindFramebuffer(GL_FRAMEBUFFER, mBuffer);

    GLuint attachments[] = { GL_COLOR_ATTACHMENT0, 
                             GL_COLOR_ATTACHMENT1,
                             GL_COLOR_ATTACHMENT2 };

    glDrawBuffers(3, attachments);
    
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

void GBuffer::endGeometryPass1()
{

}

void GBuffer::startStencilPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mBuffer);

    glDepthMask(GL_FALSE); //dont upd depth
    
    glEnable(GL_STENCIL_TEST); 
    glDrawBuffer(GL_NONE); //dont draw lights
    glEnable(GL_DEPTH_TEST); //depth test for incrementing
    glDisable(GL_CULL_FACE); //dont cull back or front faces for incrementing
    
    glClear(GL_STENCIL_BUFFER_BIT);
    
    glStencilFunc(GL_ALWAYS, 0, 0);
    
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
}

void GBuffer::endStencilPass()
{

}

void GBuffer::startLightPass()
{
    GLuint attachments2[] = { GL_COLOR_ATTACHMENT2 };

    glDrawBuffers(1, attachments2);

    glStencilFunc(GL_NOTEQUAL, 0, 0xff);
    glDisable(GL_DEPTH_TEST);

    //blend all lights drawn
    enableAdditiveBlending();
    
    //when camera inside volume, back faces must not be culled
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
}


void GBuffer::endLightPass()
{
}

void GBuffer::startGeometryPass2()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
}

void GBuffer::enableAdditiveBlending()
{
    glEnable(GL_BLEND); 
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
}

void GBuffer::endGeometryPass2(size_t w, size_t h)
{

    glDepthMask(GL_TRUE);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mBuffer);

    glBlitFramebuffer(0, 0, w, h,
                      0, 0, w, h,
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE); 

}


void GBuffer::setSSAO(SSAO& ssao)
{
    mSSAO = &ssao;
}


void GBuffer::unload()
{
    
}
