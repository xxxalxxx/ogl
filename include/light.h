#ifndef ALM_LIGHT_H
#define ALM_LIGHT_H


#include <math.h>

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"


struct Light
{
    Light();
    glm::vec3 color;
    float specPower;
   // glm::vec3 ambient, diffuse, specular;

};

struct DirectionalLight : public Light
{
    glm::vec3 direction;
};


struct PointLight : public Light
{
    PointLight();
    PointLight(float radius, const glm::vec3& color);

    void setRadius(float radius);
    float getRadius();
    
    void setAttenuation(float newQuadratic, float newLinear, float newConstant);
    glm::vec3 position;
    float quadratic, linear, constant;
};

struct SpotLight : public PointLight
{
    SpotLight();
    SpotLight(float radius);
    glm::vec3 direction;

    float cutoff, cutoffStart; 
    float exponent;
};

#endif
