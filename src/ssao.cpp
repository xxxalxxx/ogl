#include "ssao.h"

SSAO::SSAO()
{

}

SSAO::~SSAO()
{

}

bool SSAO::init(size_t w, size_t h, float scaleSSAO, float scaleBlurSSAO)
{
    if(!(w & h) || !scaleSSAO || !scaleBlurSSAO) return false;
    
    mScaleSSAO = fabs(scaleSSAO);
    mScaleBlurSSAO = fabs(scaleBlurSSAO);

    size_t wSSAO = (size_t) ((float)w*mScaleSSAO);
    size_t hSSAO  = (size_t) ((float)h*mScaleSSAO);
    
    size_t wBlurSSAO = (size_t) ((float)w*mScaleBlurSSAO);
    size_t hBlurSSAO = (size_t) ((float)h*mScaleBlurSSAO);

    initNoiseTexture();
    initKernel();

    glGenFramebuffers(1, &mBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mBuffer);

    glGenTextures(1, &mSSAO);
    glBindTexture(GL_TEXTURE_2D, mSSAO);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, wSSAO, hSSAO, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mSSAO, 0);

    glGenTextures(1, &mBlurredSSAO);
    glBindTexture(GL_TEXTURE_2D, mBlurredSSAO);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, wBlurSSAO, hBlurSSAO, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mBlurredSSAO, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG("ERROR:: SSAO FRAMEBUFFER NOT COMPLETE " << status);
        return false;
    }
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return true;
}

void SSAO::initKernel()
{
    size_t numSamples = 32;
    mKernelSamples.clear();
    mKernelSamples.reserve(numSamples);

  std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
  std::default_random_engine generator;

    for (GLuint i = 0; i < numSamples; ++i)
    { 
        glm::vec3 sample(
                randomFloats(generator) * 2.0 - 1.0, 
                randomFloats(generator) * 2.0 - 1.0, 
                randomFloats(generator));

        LOG( "KERNEL:" << glm::to_string(sample));
        sample = glm::normalize(sample);
        sample *= Utils::randF(0.0f, 1.0f);
        GLfloat scale = GLfloat(i) / ((float)numSamples);
        scale = 0.1f + (scale * scale) * (1.0f - 0.1f);
        sample *= scale;
        mKernelSamples.push_back(sample);
    }
}


void SSAO::initNoiseTexture()
{
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
    std::default_random_engine generator;

    std::vector<glm::vec3> noise;
    noise.reserve(16);
    for (GLuint i = 0; i < 16; i++)
    {
        glm::vec3 noiseVec(
                randomFloats(generator) * 2.0 - 1.0, 
                randomFloats(generator) * 2.0 - 1.0, 
                0.0f);

        LOG( "NOISE:" << glm::to_string(noiseVec));

        noise.push_back(noiseVec);
    }

    glGenTextures(1, &mNoise);
    glBindTexture(GL_TEXTURE_2D, mNoise);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &noise[0]);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

std::vector<GLuint> SSAO::getUniformSampleHandles(GLuint program)
{
    std::vector<GLuint> kernelSampleHandles;
    kernelSampleHandles.reserve(mKernelSamples.size());
    for(size_t i=0;i<mKernelSamples.size();++i)
    {
        std::string name = "u_Kernel[" + std::to_string(i) + "]";
        GLuint handle = glGetUniformLocation(program, name.c_str());
        kernelSampleHandles.push_back(handle);
    }
    return kernelSampleHandles;
}

void SSAO::setUniformSampleHandles(std::vector<GLuint>& kernelSampleHandles)
{
   for(size_t i=0;i<mKernelSamples.size();++i)
   {
        glUniform3fv(kernelSampleHandles[i], 1, glm::value_ptr(mKernelSamples[i]));
   } 
}

void SSAO::startSSAOPass()
{
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, mBuffer);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


    GLuint attachmentsSSAO[] = { GL_COLOR_ATTACHMENT0 };

    glDrawBuffers(1, attachmentsSSAO);

}

void SSAO::startSSAOBlurPass()
{

    GLuint attachmentSSAOBlur[] = { GL_COLOR_ATTACHMENT1 };

    glDrawBuffers(1, attachmentSSAOBlur);


}

