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
    
    GLuint attachments[] = { GL_COLOR_ATTACHMENT0, 
                             GL_COLOR_ATTACHMENT1, 
                             GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    glGenRenderbuffers(1, &mDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, mDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepth);


    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG("ERROR:: FRAMEBUFFER NOT COMPLETE " << status);
        return false;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void GBuffer::unload()
{
    
}
