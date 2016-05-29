#include "LightManager.h"

PointLight::PointLight(): a2(0.032f), a1(0.09f), a0(1.0f)
{

}

SpotLight::SpotLight(): cutoff(M_PI_4), cutoffStart(0.66f * M_PI_4)
{

}


void PointLight::setAttenuation(float newA2, float newA1, float newA0)
{
    a2 = newA2;
    a1 = newA1;
    a0 = newA0;
}

const size_t LightManager::MAX_SPOT_LIGHTS = 8;
const size_t LightManager::MAX_POINT_LIGHTS = 8;

LightManager::LightManager()
{

}

LightManager::~LightManager()
{

}

LightManager& LightManager::getInstance()
{
    static LightManager lm;
    return lm;
}
