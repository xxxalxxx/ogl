#ifndef ALM_TEXTURE_MANAGER
#define ALM_TEXTURE_MANAGER

#include <string.h>

#include <iostream>
#include <string>
#include <unordered_map>

#include <GL/glew.h> 

#include "SOIL/SOIL.h"

#define LOG(x) std::cout<< x << std::endl

class TextureManager
{
public:
    static TextureManager& getInstance();  

    GLuint load(const char* path, const char* relativeTexFilePath, bool gamma = false);
    GLuint load(const std::string& path, const std::string& relativeTexFilePath, bool gamma = false);
    GLuint load(const std::string& path, bool gamma = false);
    GLuint load(const char* path, bool gamma = false);

    void unload();

private:
    void checkGlTextureFormatFromSOIL(bool* isValidChannel, GLenum* out, unsigned int channel);
    std::string getPathString(const std::string& path, const std::string& relativeTexFilePath);

    TextureManager(){}
    ~TextureManager(){unload();}
    TextureManager(const TextureManager& other);  
    TextureManager& operator=(TextureManager& other);

    typedef std::pair<std::string, GLuint> tex_pair_t;
    typedef std::unordered_map<std::string, GLuint> tex_container_t;
    typedef tex_container_t::iterator tex_iter_t;

    tex_container_t mCurrTextureHandles;

};

inline TextureManager& TextureManager::getInstance()
{
    static TextureManager textureManager;
    return textureManager;      
}


inline void TextureManager::checkGlTextureFormatFromSOIL(bool* isValidChannel, GLenum* out, unsigned int channel)
{
    //param channels the number of channels: 1-luminous, 2-luminous/alpha, 3-RGB, 4-RGBA
    *isValidChannel = true;
    switch(channel)
    {
        case 1: *out = GL_RED;  break;
        case 2: *out = GL_RG;   break;
        case 3: *out = GL_RGB;  break;
        case 4: *out = GL_RGBA; break;
        default: *isValidChannel = false;
    }

}

inline std::string TextureManager::getPathString(const std::string& path, const std::string& relativeTexFilePath)
{
    std::string ret(path);
    if(ret[ret.size() - 1] != '/') ret += '/';
    ret.append(relativeTexFilePath);


    return ret;
}



inline GLuint TextureManager::load(const char* path, const char* relativeTexFilePath, bool gamma/* = false */)
{
    std::string pathStr(path);
    std::string relativeTexFilePathStr(relativeTexFilePath);
    
    return load(pathStr, relativeTexFilePathStr, gamma);
}

inline GLuint TextureManager::load(const std::string& path, const std::string& relativeTexFilePath, bool gamma/* = false */)
{
    std::string filepath( getPathString(path, relativeTexFilePath) );
    
    return load(filepath, gamma);
}

inline GLuint TextureManager::load(const char* path, bool gamma /* = false */)
{
    std::string pathStr(path);
    return load(pathStr, gamma);
}



inline GLuint TextureManager::load(const std::string& path, bool gamma/*  = false */)
{
    LOG("BEFORE PATH");
    LOG(path);

    tex_iter_t it = mCurrTextureHandles.find(path);

    if(it != mCurrTextureHandles.end())
    {
        LOG("TEX EXISTS");
        return it->second;
    }

    GLuint textureId;
    glGenTextures(1, &textureId);
    int w, h;
    int channels;

 //   LOG("BEFORE LOAD");
    unsigned char* image = SOIL_load_image(path.c_str(), &w, &h, &channels, SOIL_LOAD_AUTO);
    
//    LOG("AFTER LOAD");
    GLenum format;
    bool validChannel;
    checkGlTextureFormatFromSOIL(&validChannel, &format, channels);
    LOG(channels);
    if(!validChannel)
    {
        LOG("NOT VALID CHANNED");
        format = gamma ? GL_SRGB : GL_RGB; 
    }
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureId);
//    LOG("AFTER BIND");
    glTexImage2D(GL_TEXTURE_2D, 0, format,  w, h, 0, format, GL_UNSIGNED_BYTE, image);
//    LOG("AFTER TEX IMAGE");
    glGenerateMipmap(GL_TEXTURE_2D);	

  //  LOG("AFTER MIP");
    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
 //   LOG("AFTER TEX");

    mCurrTextureHandles.insert(tex_pair_t(path,textureId ));
    return textureId;
}


void TextureManager::unload()
{
    tex_iter_t it;
    for(it = mCurrTextureHandles.begin(); it != mCurrTextureHandles.end(); ++it )
    {
        glDeleteTextures(1, &it->second);
    }
    mCurrTextureHandles.clear();
}













#endif
