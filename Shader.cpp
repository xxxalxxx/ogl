#include "Shader.h"


Shader::Shader()
{

}

bool Shader::init(const GLchar* vertexShaderPath, const GLchar* fragmentShaderPath)
{ 
    GLint status;
    bool result = true;
    const char* vertexShaderString = getFileString(vertexShaderPath);

    GLuint vertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderHandle, 1, &vertexShaderString, NULL);
    glCompileShader(vertexShaderHandle);
    delete[] vertexShaderString;

    glGetShaderiv(vertexShaderHandle, GL_COMPILE_STATUS , &status);

    if(status == GL_FALSE)
    {
        logCompileError(vertexShaderHandle);
        result = false;  
    }
    


    const char* fragmentShaderString = getFileString(fragmentShaderPath);
    
    GLuint fragmentShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderHandle, 1, &fragmentShaderString, NULL);
    glCompileShader(fragmentShaderHandle);
    delete[] fragmentShaderString;

    glGetShaderiv(fragmentShaderHandle, GL_COMPILE_STATUS , &status);

    if(status == GL_FALSE)
    {
        logCompileError(fragmentShaderHandle);
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

char* Shader::getFileString(const GLchar* path)
{
    FILE* file = fopen(path,"r");
    
    fseek(file,0, SEEK_END);
    size_t size = ftell(file);
    
    char* fileString = new char[size];

    rewind(file);
    fread(fileString, sizeof(char), size, file);
    fclose(file);

    return fileString;
}

void Shader::logCompileError(GLuint handle)
{
     GLint infoLogLength;
     glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
     GLchar* infoLog = new char[infoLogLength];
     glGetShaderInfoLog(handle, infoLogLength, NULL, infoLog);
     std::cout<< "Shader compile error:" << std::endl << infoLog << std::endl;
     delete[] infoLog;
}

