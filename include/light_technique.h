#ifndef LIGHT_TECHNIQUE_H
#define LIGHT_TECHNIQUE_H

#include <GL/glew.h> 

struct LightTechnique
{
    LightTechnique(){}
    GLuint handleAmbient, handleSpecular, handleDiffuse;
};

struct PointLightTechnique : public LightTechnique
{
    PointLightTechnique(){}
    GLuint handleA0, handleA1, handleA2;
    GLuint handlePosition;
};

struct DirectionalLightTechnique : public LightTechnique
{
    DirectionalLightTechnique(){}
    GLuint handleDirection;
};

struct SpotLightTechnique : public PointLightTechnique 
{
    SpotLightTechnique(){}
    GLuint handleCutoff, handleCutoffStart, handleExponent;
    GLuint handleDirection;
};

#endif
