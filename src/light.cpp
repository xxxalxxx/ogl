#include "light.h"

PointLight::PointLight(): a2(0.032f), a1(0.09f), a0(1.0f)
{

}

PointLight::PointLight(float radius)
{
    setRadius(radius);
}

void PointLight::setRadius(float radius)
{
    float r2 = radius * radius;
    //cut off light at 1/255
    float newA2 = 255.0f/r2;
    setAttenuation(newA2, 0.0f, 1.0f);
}

SpotLight::SpotLight(): cutoff(M_PI_4), cutoffStart(0.66f * M_PI_4)
{

}

SpotLight::SpotLight(float radius): PointLight(radius), cutoff(M_PI_4), cutoffStart(0.66f * M_PI_4)
{

}

void PointLight::setAttenuation(float newA2, float newA1, float newA0)
{
    a2 = newA2;
    a1 = newA1;
    a0 = newA0;
}


