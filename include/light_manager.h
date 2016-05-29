#ifndef ALM_LIGHT_H
#define ALM_LIGHT_H

#include <set>

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"


class LightManager 
{
public:
    LightManager();
    ~LightManager();
    
    LightManager& getInstance();

    void unload();

    static const size_t MAX_SPOT_LIGHTS;
    static const size_t MAX_POINT_LIGHTS;

    bool addTechnique(Technique* tech);
    bool removeTechnique(Technique* tech);

    std::set<Technique*> mTechniques;

private:
    LightManager();
    ~LightManager();
    LightManager(const LightManager& other);  
    LightManager& operator=(LightManager& other);

};


struct Light
{
    glm::vec3 ambient, diffuse, specular;

    LightManager* mLightManager;

    void setAmbient(const glm::vec3& ambient);
    void setDiffuse(const glm::vec3& diffuse);
    void setSpecular(const glm::vec3& specular);

};

struct DirectionalLight : public Light
{
    glm::vec3 direction;
    
    void setDirection(const glm::vec3& direction);
};


struct PointLight : public Light
{
    void setAttenuation(float newA2, float newA1, float newA0);
    void setPosition(const glm::vec3& position);

    glm::vec3 position;
    float a2, a1, a0;
};

struct SpotLight : public PointLight
{
    glm::vec3 direction;
    void setDirection(const glm::vec3& direction);

    float cutoff, cutoffStart; 
    float exponent;
};

#endif
