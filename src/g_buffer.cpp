#include "g_buffer.h"


GBuffer::GBuffer() : mBuffer(0), mPosition(0), mNormal(0), mDepth(0)
{

}

GBuffer::~GBuffer()
{
    
}

bool GBuffer::init(size_t w, size_t h)
{
    if(!w || !h) return false;

    glGenBuffers(1, &mBuffer);
    glBindBuffer(GL_FRAMEBUFFER, mBuffer);
    
    glGenTextures(1, &mPosition);
    glBindTexture(GL_TEXTURE_2D, mPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mPosition, 0);

    glGenTextures(1, &mNormal);
    glBindTexture(GL_TEXTURE_2D, mNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mNormal, 0);
    
    glGenTextures(1, &mColor);
    glBindTexture(GL_TEXTURE_2D, mColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mColor, 0);

    glGenTextures(1, &mDiffuse);
    glBindTexture(GL_TEXTURE_2D, mDiffuse);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, mDiffuse, 0);
    
    glGenTextures(1, &mSpecular);
    glBindTexture(GL_TEXTURE_2D, mSpecular);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, mSpecular, 0);


    glGenTextures(1, &mDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepth, 0);
/*
    glGenRenderbuffers(1, &mDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, mDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepth);
*/

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG("ERROR:: FRAMEBUFFER NOT COMPLETE " << status);
        return false;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void GBuffer::startGeometryPass1()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mBuffer);

    GLuint attachments[] = { GL_COLOR_ATTACHMENT0, 
                             GL_COLOR_ATTACHMENT1};

    glDrawBuffers(2, attachments);
    
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

void GBuffer::endGeometryPass1()
{
    glDepthMask(GL_FALSE);
   // glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::startStencilPass()
{
    glEnable(GL_STENCIL_TEST);
    glDrawBuffer(GL_NONE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
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
    GLuint attachments[] = { GL_COLOR_ATTACHMENT3, 
                             GL_COLOR_ATTACHMENT4 };

    glDrawBuffers(2, attachments);
    //TODO: add uniform samplers?
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mPosition);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mNormal);
   
    glStencilFunc(GL_NOTEQUAL, 0, 0xff);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
}


void GBuffer::endLightPass()
{
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
}

void GBuffer::startGeometryPass2()
{
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mPosition);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mNormal);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mColor);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, mDiffuse);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, mSpecular);


   /* glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);*/
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

void GBuffer::endGeometryPass2(size_t w, size_t h)
{
 //   glDisable(GL_BLEND);
/*    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mBuffer);
    
    glBlitFramebuffer(0, 0, w, h,
                      0, 0, w, h,
                      GL_DEPTH_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);  */ 
}

void GBuffer::enableAdditiveBlending()
{
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
}

void GBuffer::enableTextures()
{
    GLuint attachments[] = { GL_COLOR_ATTACHMENT0, 
                             GL_COLOR_ATTACHMENT1,
                             GL_COLOR_ATTACHMENT2, 
                             GL_COLOR_ATTACHMENT3 };

    glDrawBuffers(2, attachments);
    //TODO: add uniform samplers?
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mPosition);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mNormal);

}

void GBuffer::unload()
{
    
}
