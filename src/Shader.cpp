#include "Shader.h"


Shader::Shader()
{

}

bool Shader::init(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    return init(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
}

bool Shader::init(const GLchar* vertexShaderPath, const GLchar* fragmentShaderPath)
{ 
    GLint status;
    bool result = true;
    FileSystem& fs = FileSystem::getInstance();

    const char* vertexShaderString = fs.getFileString(vertexShaderPath);
    if(!vertexShaderString)
    {
        std::cout << "File at path "<< vertexShaderPath << " not found." << std::endl;
        return false;
    }
  
    GLuint vertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderHandle, 1, &vertexShaderString, NULL);
    glCompileShader(vertexShaderHandle);
    delete[] vertexShaderString;

    glGetShaderiv(vertexShaderHandle, GL_COMPILE_STATUS , &status);

    if(status == GL_FALSE)
    {
        logCompileError(vertexShaderHandle, "Vertex Shader::");
        result = false;  
    }
    
    const char* fragmentShaderString = fs.getFileString(fragmentShaderPath);
 
    if(!vertexShaderString)
    {
        std::cout << "File at path "<< fragmentShaderPath << " not found." << std::endl;
        return false;
    }

    GLuint fragmentShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderHandle, 1, &fragmentShaderString, NULL);
    glCompileShader(fragmentShaderHandle);
    delete[] fragmentShaderString;

    glGetShaderiv(fragmentShaderHandle, GL_COMPILE_STATUS , &status);

    if(status == GL_FALSE)
    {
        logCompileError(fragmentShaderHandle, "Fragment Shader::");
        result = false;
    }

    mProgram = glCreateProgram();
    glAttachShader(mProgram, vertexShaderHandle);
    glAttachShader(mProgram, fragmentShaderHandle);
    glLinkProgram(mProgram);

    glGetProgramiv(mProgram, GL_LINK_STATUS, &status);

    if(status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* infoLog = new char[infoLogLength];
        glGetProgramInfoLog(mProgram, infoLogLength, NULL, infoLog);
        std::cout<< "Program compile error:" << std::endl << infoLog << std::endl;
        delete[] infoLog;
        result = false;
    }
    

    glDeleteShader(vertexShaderHandle);
    glDeleteShader(fragmentShaderHandle);
    

    return result;
}

void Shader::logCompileError(GLuint handle, const char* tag)
{
     GLint infoLogLength;
     glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
     GLchar* infoLog = new char[infoLogLength];
     glGetShaderInfoLog(handle, infoLogLength, NULL, infoLog);
     std::cout << tag << "shader compile error:" << std::endl << infoLog << std::endl;
     delete[] infoLog;
}

