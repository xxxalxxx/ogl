#ifndef LIGHT_TECHNIQUE_H
#define LIGHT_TECHNIQUE_H

#include <GL/glew.h> 

struct LightTechnique
{
    LightTechnique(){}
   // GLuint handleAmbient, handleSpecular, handleDiffuse;
    GLuint handleColor;
    GLuint handleSpecPower;
};

struct PointLightTechnique : public LightTechnique
{
    PointLightTechnique(){}
    GLuint handleQuadratic, handleLinear, handleConstant;
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
