#include "light.h"

Light::Light(): specPower(1.0f)
{

}

PointLight::PointLight(): quadratic(0.032f), linear(0.09f), constant(1.0f)
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
    float newQuadratic = 255.0f/r2;
    setAttenuation(newQuadratic, 0.0f, 1.0f);
}

SpotLight::SpotLight(): cutoff(M_PI_4), cutoffStart(0.66f * M_PI_4)
{

}

SpotLight::SpotLight(float radius): PointLight(radius), cutoff(M_PI_4), cutoffStart(0.66f * M_PI_4)
{

}

void PointLight::setAttenuation(float newQuadratic, float newLinear, float newConstant)
{
    quadratic = newQuadratic;
    linear = newLinear;
    constant = newConstant;
}


