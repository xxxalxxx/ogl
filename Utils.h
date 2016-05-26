#ifndef ALM_UTILS
#define ALM_UTILS

#include <string>

class Utils
{
public:
    static std::string getPathFromFileName(const std::string& fileName);
    static void clamp(float& num, float min, float max);
};

inline std::string Utils::getPathFromFileName(const std::string& fileName)
{
    size_t slashPos = fileName.find_last_of("/\\");
    if(slashPos == std::string::npos) return "";
    return fileName.substr(0, slashPos);
}

inline void Utils::clamp(float& num, float min, float max)
{
    if(num < min) num = min;
    else if(num > max) num = max; 
}


#endif
