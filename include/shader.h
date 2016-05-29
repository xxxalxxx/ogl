#ifndef ALM_SHADER_H
#define ALM_SHADER_H

#include <stdio.h>

#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "utils.h"
#include "file_system.h"

class Shader
{
public:
    Shader();
    bool init(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    bool init(const GLchar* vertexShaderPath, const GLchar* fragmentShaderPath);
    void use();
    void stop();
    GLuint getProgram() const;
    
private:
    void logCompileError(GLuint handle, const char* tag);

    GLuint mProgram;
};

inline void Shader::use()
{
    glUseProgram(mProgram);
}

inline void Shader::stop()
{
    glUseProgram(0);
}

inline GLuint Shader::getProgram() const
{
    return mProgram;
}

#endif
