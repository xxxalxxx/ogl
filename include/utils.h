#ifndef ALM_UTILS_H
#define ALM_UTILS_H

#include <iostream>
#include <GL/glew.h>
#include <cstdlib>
#include <ctime>
#include "glm/vec3.hpp"

#define LOG(x) std::cout << x << std::endl;

namespace Utils
{

inline void clamp(float& num, float min, float max)
{
    if(num < min) num = min;
    else if(num > max) num = max; 
}

inline float randF(float min, float max)
{
    return min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max-min)));
}



inline glm::vec3 getRandVec3(float min, float max)
{
    return  glm::vec3(Utils::randF(min, max),Utils::randF(min, max),Utils::randF(min, max));

}

}

#endif
