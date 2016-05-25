#ifndef ALM_UTILS
#define ALM_UTILS

#include <string>

class Utils
{
public:
    static std::string getPathFromFileName(const std::string& fileName);
};

inline std::string Utils::getPathFromFileName(const std::string& fileName)
{
    size_t slashPos = fileName.find_last_of("/\\");
    if(slashPos == std::string::npos) return "";
    return fileName.substr(0, slashPos);
}


#endif