#ifndef ALM_UTILS_H
#define ALM_UTILS_H

#include <iostream>
#include <GL/glew.h>
#define LOG(x) std::cout << x << std::endl;


namespace Utils
{

inline void clamp(float& num, float min, float max)
{
    if(num < min) num = min;
    else if(num > max) num = max; 
}

} 



#endif
