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

float PointLight::getRadius()
{
    float maxColor = fmax(fmax(color.x, color.y), color.z);
    float c = constant - 256 * maxColor;
    return (-linear + sqrtf(linear*linear - 4 * quadratic * c))/(2.0f*quadratic);
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


