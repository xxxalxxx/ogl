#include "texture_manager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

const GLenum TextureManager::mFormats[5] = {0, GL_RED, GL_RG, GL_RGB, GL_RGBA };


TextureManager::TextureManager(): mFileSystem(FileSystem::getInstance())
{
}

TextureManager::~TextureManager()
{
    unload();
}

TextureManager& TextureManager::getInstance()
{
    static TextureManager textureManager;
    return textureManager;      
}



GLuint TextureManager::load(const char* path, const char* relativeTexFilePath, bool gamma/* = false */)
{
    std::string pathStr(path);
    std::string relativeTexFilePathStr(relativeTexFilePath);
    
    return load(pathStr, relativeTexFilePathStr, gamma);
}

GLuint TextureManager::load(const std::string& path, const std::string& relativeTexFilePath, bool gamma/* = false */)
{
    std::string filepath(mFileSystem.getPathString(path, relativeTexFilePath) );
    
    return load(filepath, gamma);
}

GLuint TextureManager::load(const char* path, bool gamma /* = false */)
{
    std::string pathStr(path);
    return load(pathStr, gamma);
}

GLuint TextureManager::load(const std::string& path, bool gamma/* = false */)
{
    LOG("BEFORE PATH");
    LOG(path);
    
    tex_iter_t it = mCurrTextureHandles.find(path);

    if(it != mCurrTextureHandles.end())
    {
        LOG("TEX EXISTS:" << it->second);
        return it->second;
    }

    GLuint textureId;
    ImageData id;

    outImageData(path.c_str(), id);   
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, id.format, id.w, id.h, 0, id.format, GL_UNSIGNED_BYTE, id.img);
   
    glGenerateMipmap(GL_TEXTURE_2D);	
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    mCurrTextureHandles.insert(tex_pair_t(path, textureId));
    freeImage(id);
    LOG("TEXID:" << textureId);
    return textureId;
}

GLuint TextureManager::loadCubeMap(
            const std::string& posX, const std::string& negX, 
            const std::string& posY, const std::string& negY, 
            const std::string& posZ, const std::string& negZ, 
            bool gamma /* = false */)
{
    GLuint textureId;

    glGenTextures(1, &textureId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

    loadCubeMapFace(posX, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
    loadCubeMapFace(negX, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    
    loadCubeMapFace(posY, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    loadCubeMapFace(negY, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    
    loadCubeMapFace(posZ, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
    loadCubeMapFace(negZ, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureId;
}


void TextureManager::loadCubeMapFace(const std::string& path, GLenum face, bool gamma /* = false */)
{
    ImageData id;
    outImageData(path.c_str(), id);
    glTexImage2D(face, 0, id.format, id.w, id.h, 0, id.format, GL_UNSIGNED_BYTE, id.img);
    freeImage(id);   

}


void TextureManager::outImageData(const char* path, ImageData& id)
{
    id.img = stbi_load(path, &(id.w), &(id.h), &(id.channels), 0);

    if(!id.channels || !id.w || !id.h)
    {
        LOG("ERROR: " << stbi_failure_reason() << "path:" << path  << " ch:"<< id.channels << " w:" << id.w << "h:" << id.h);
        exit(1);
    }

    id.format = mFormats[id.channels];
  //  LOG("CHANNELS:" << id.channels);
}

void TextureManager::freeImage(ImageData& id)
{
    stbi_image_free(id.img);
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


