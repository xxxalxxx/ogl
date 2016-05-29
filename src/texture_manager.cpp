#include "texture_manager.h"

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

GLuint TextureManager::load(const std::string& path, bool gamma/*  = false */)
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
    GLenum formats[] = {0, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA };
    int w=0, h=0, channels=0;
    GLenum format;
    
    unsigned char* img =  stbi_load(path.c_str(), &w, &h, &channels, 0);// SOIL_load_image(path.c_str(), &w, &h, &channels, SOIL_LOAD_AUTO); 

    LOG("AFTER LOAD:" << channels << " w:" << w << " h" << h);
    if(!channels || !w || !h)
    {
        LOG("ERROR: " << stbi_failure_reason()  << " ch:"<< channels << " w:" << w << "h:" << h);
        exit(1);
    }
    format = formats[channels];

   
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, img);
   
    glGenerateMipmap(GL_TEXTURE_2D);	
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    mCurrTextureHandles.insert(tex_pair_t(path, textureId ));
    stbi_image_free(img);

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


/*ilGenImages(1, &imgId);

ilBindImage(imgId);

ilEnable(IL_ORIGIN_SET);
ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

ilLoadImage(path.c_str());

ilConvertImage(IL_RGB, GL_UNSIGNED_BYTE); 

img = ilGetData();
w = ilGetInteger(IL_IMAGE_WIDTH);
h = ilGetInteger(IL_IMAGE_HEIGHT);
type = ilGetInteger(IL_IMAGE_TYPE);
format = ilGetInteger(IL_IMAGE_FORMAT);
ilBindImage(0);
ilDeleteImage(imgId);*/
