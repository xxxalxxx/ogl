#ifndef ALM_SSAO_H
#define ALM_SSAO_H

#include <vector>
#include <string>
#include <random>
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "utils.h"

class SSAO
{
public:
    SSAO();
    virtual ~SSAO();

    bool init(size_t w, size_t h, float scaleSSAO = 1.0f, float scaleBlurSSAO = 1.0f);
    void initNoiseTexture();
    void initKernel();

    std::vector<GLuint> getUniformSampleHandles(GLuint program);
    void setUniformSampleHandles(std::vector<GLuint>& kernelSampleHandles);
    void startSSAOPass();
    void startSSAOBlurPass();

    GLuint mBuffer, mSSAO, mBlurredSSAO, mNoise;
    float mScaleSSAO, mScaleBlurSSAO;
    std::vector<glm::vec3> mKernelSamples;

};


#endif
