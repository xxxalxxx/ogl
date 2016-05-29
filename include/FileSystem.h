#ifndef ALM_FILESYSTEM_H
#define ALM_FILESYSTEM_H

#include <limits.h>
#include <stdio.h>

#ifdef WINDOWS
    #include <direct.h>
    #define getCurrentDir _getcwd
#else
    #include <unistd.h>
    #define getCurrentDir getcwd
#endif

#include <string>

#include "Utils.h"

class FileSystem
{
public:

    static FileSystem& getInstance();

    std::string getReplacedTexturePathFileName(const char* from, const std::string& to);
    std::string getReplacedTexturePathFileName(const std::string& from, const std::string& to);
    std::string getPathString(const std::string& path, const std::string& relativeTexFilePath);
    std::string getPathFromFileName(const std::string& fileName);
    std::string getAbsPath(const std::string& fileName);
    std::string getBinAbsPath();
    std::string getCurrPath();
    std::string getStrippedFileName(const char* fileName);
    std::string getStrippedFileName(const std::string& fileName);
    char* getFileString(const char* path);

private:
    FileSystem();
    ~FileSystem();
    FileSystem(const FileSystem& other);  
    FileSystem& operator=(FileSystem& other);



};


#endif
