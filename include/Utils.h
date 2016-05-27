#ifndef ALM_UTILS
#define ALM_UTILS

#include <iostream>

#define LOG(x) std::cout << x << std::endl;


class Utils
{
public:
    static void clamp(float& num, float min, float max);
}; 

inline void Utils::clamp(float& num, float min, float max)
{
    if(num < min) num = min;
    else if(num > max) num = max; 
}


#endif