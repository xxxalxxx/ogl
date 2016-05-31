#ifndef ALM_LIGHT_H
#define ALM_LIGHT_H


#include <math.h>

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"


struct Light
{
    glm::vec3 ambient, diffuse, specular;

};

struct DirectionalLight : public Light
{
    glm::vec3 direction;
};


struct PointLight : public Light
{
    PointLight();
    PointLight(float radius);

    void setRadius(float radius);
    
    void setAttenuation(float newA2, float newA1, float newA0);
    glm::vec3 position;
    float a2, a1, a0;
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
