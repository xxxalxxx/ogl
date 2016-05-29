#include <FileSystem.h>

FileSystem::FileSystem()
{
}

FileSystem::~FileSystem()
{
}

FileSystem& FileSystem::getInstance()
{
    static FileSystem fileSystem;
    return fileSystem;
}

std::string FileSystem::getReplacedTexturePathFileName(const char* from, const std::string& to)
{
    std::string s(from);
    return getReplacedTexturePathFileName(s, to);
}

std::string FileSystem::getReplacedTexturePathFileName(const std::string& fromFileName, const std::string& to)
{
    std::string path = getPathFromFileName(fromFileName) + '/';
    size_t start_pos = fromFileName.find(path);

    if(start_pos == std::string::npos)
    {
        return fromFileName;
    }

    std::string ret = fromFileName;
    ret.replace(start_pos, path.length(), to);

    return ret;
}

std::string FileSystem::getPathString(const std::string& path, const std::string& relativeTexFilePath)
{
    LOG("IN PATH STR");
    std::string ret(path);
    if(ret[ret.size() - 1] != '/') ret += '/';
    ret.append(relativeTexFilePath);
    LOG("PSTR:" << ret);
    return ret;
}

std::string FileSystem::getBinAbsPath()
{
    char buff[PATH_MAX];
    size_t sz = sizeof(buff)-1;
    LOG("IN BIN ABS PATH");
    #ifdef WINDOWS
        if(!GetModuleFileName(NULL, buff, sz))
        {
            LOG("ERROR: COULD NOT READ EXE PATH");
            exit(1);
        }
    #else 
        ssize_t len;
        if((len = readlink("/proc/self/exe", buff, sz)) == -1)
        {
            LOG("ERROR: COULD NOT READ EXE PATH");
            exit(1);
        }
        buff[len] = '\0';
    #endif
    LOG("B ABS:" << std::string(buff));
    return std::string(buff);
}

std::string FileSystem::getAbsPath(const std::string& fileName)
{
    LOG("GET ABS P");
    std::string binPath = getPathFromFileName(getBinAbsPath());
    binPath.append("/../");
    binPath.append(fileName);
    LOG("BIN P:" << binPath);
    return binPath;
}

std::string FileSystem::getPathFromFileName(const std::string& fileName)
{
    LOG("P FROM F:"<< fileName);
    size_t slashPos = fileName.find_last_of("/\\");
    if(slashPos == std::string::npos) return "";
    LOG( "PFR:" <<  fileName.substr(0,slashPos));
    return fileName.substr(0, slashPos);
}


char* FileSystem::getFileString(const char* path)
{
    FILE* file = fopen(path,"r");

    if(!file)
    {
        LOG("FILE \""<<path << "\" NOT FOUND");
        exit(1);
    }

    fseek(file,0, SEEK_END);
    size_t size = ftell(file);
    
    char* fileString = new char[size + 1];

    rewind(file);
    size_t result = fread(fileString, sizeof(char), size, file);
    if(result != size)
    {
        LOG("ERROR WHILE READING \""<< path <<"\"");
        delete[] fileString;
        exit(1);
    }

    fclose(file);
    fileString[result - 1] = '\0';
    return fileString;
}

std::string FileSystem::getStrippedFileName(const char* fileName)
{
    std::string fileNameStr(fileName);
    return getStrippedFileName(fileNameStr);
}

std::string FileSystem::getStrippedFileName(const std::string& fileName)
{
    return fileName.substr(fileName.find_last_of("/\\") + 1);    
}
