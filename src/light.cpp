#include "light.h"

#include "utils.h"

Light::Light(): specPower(1.0f)
{

}

PointLight::PointLight(): quadratic(0.032f), linear(0.09f), constant(1.0f)
{

}

PointLight::PointLight(float radius, const glm::vec3& col)
{
    color = col;
    setRadius(radius);
}

void PointLight::setRadius(float radius)
{
    float r2 = radius * radius;
    float maxColor = fmax(fmax(color.x, color.y), color.z);

    float newQuadratic = (256.0f*maxColor - 1.0f)/r2;
    setAttenuation(newQuadratic, 0.0f, 1.0f);
   // LOG("RAD:" << radius <<" RE RAD:" << getRadius());
}

float PointLight::getRadius()
{
    float maxColor = fmax(fmax(color.x, color.y), color.z);
    float c = constant - 256.0f  * maxColor;
   // LOG("m:"<< maxColor << "c:" << c << "l:" << linear << "q:" << quadratic); 
    return (-linear + sqrtf(linear*linear - 4 * quadratic * c))/(2.0f*quadratic);
}

SpotLight::SpotLight(): cutoff(M_PI_4), cutoffStart(0.66f * M_PI_4)
{

}

SpotLight::SpotLight(float radius): cutoff(M_PI_4), cutoffStart(0.66f * M_PI_4)
{

}

void PointLight::setAttenuation(float newQuadratic, float newLinear, float newConstant)
{
    quadratic = newQuadratic;
    linear = newLinear;
    constant = newConstant;
}


