#ifndef ALM_TEXTURE_MANAGER_H
#define ALM_TEXTURE_MANAGER_H

#include <string.h>

#include <iostream>
#include <string>
#include <unordered_map>

#include <GL/glew.h> 


#include "utils.h"
#include "file_system.h"


struct ImageData
{
    int w, h, channels;

    ImageData(): w(0), h(0), channels(0) 
    {
    }

    GLenum format;
    unsigned char* img;
};

class TextureManager
{
public:
    static TextureManager& getInstance();  

    GLuint load(const char* path, const char* relativeTexFilePath, bool gamma = false);
    GLuint load(const std::string& path, const std::string& relativeTexFilePath, bool gamma = false);
    GLuint load(const std::string& path, bool gamma = false);
    GLuint load(const char* path, bool gamma = false);
    GLuint loadCubeMap(
            const std::string& posX, const std::string& negX, 
            const std::string& posY, const std::string& negY, 
            const std::string& posZ, const std::string& negZ, 
            bool gamma = false);
    void loadCubeMapFace(const std::string& path, GLenum face, bool gamma = false);

    void outImageData(const char* path, ImageData& imgData);
    void freeImage(ImageData& imgData);


    void unload();

private:

    TextureManager();
    ~TextureManager();
    TextureManager(const TextureManager& other);  
    TextureManager& operator=(TextureManager& other);
    // <path, handle>
    typedef std::pair<std::string, GLuint> tex_pair_t;
    typedef std::unordered_map<std::string, GLuint> tex_container_t;
    typedef tex_container_t::iterator tex_iter_t;

    const static GLenum mFormats[5];

    tex_container_t mCurrTextureHandles;
    FileSystem& mFileSystem;

};
#endif
