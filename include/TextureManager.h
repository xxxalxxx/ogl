#ifndef ALM_TEXTURE_MANAGER
#define ALM_TEXTURE_MANAGER

#include <string.h>

#include <iostream>
#include <string>
#include <unordered_map>

#include <GL/glew.h> 

#include <IL/il.h>
#include "Utils.h"
#include "FileSystem.h"

class TextureManager
{
public:
    static TextureManager& getInstance();  

    GLuint load(const char* path, const char* relativeTexFilePath, bool gamma = false);
    GLuint load(const std::string& path, const std::string& relativeTexFilePath, bool gamma = false);
    GLuint load(const std::string& path, bool gamma = false);
    GLuint load(const char* path, bool gamma = false);

    void unload();

    enum TexFlags 
    {
        hasGamma = 1,
        stripFileNamePath = 2
    };

private:

    TextureManager();
    ~TextureManager();
    TextureManager(const TextureManager& other);  
    TextureManager& operator=(TextureManager& other);

    typedef std::pair<std::string, GLuint> tex_pair_t;
    typedef std::unordered_map<std::string, GLuint> tex_container_t;
    typedef tex_container_t::iterator tex_iter_t;

    tex_container_t mCurrTextureHandles;
    FileSystem& mFileSystem;

};
#endif
