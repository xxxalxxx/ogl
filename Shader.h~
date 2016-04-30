#ifndef ALM_SHADER_H
#define ALM_SHADER_H

#include <stdio.h>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Shader
{
public:
    Shader();
    bool init(const GLchar* vertexShaderPath, const GLchar* fragmentShaderPath);
    void use();
    GLuint getProgram();
private:
    char* getFileString(const GLchar* path);
    void logCompileError(GLuint handle);

    GLuint mProgram;
};

inline void Shader::use()
{
    glUseProgram(mProgram);
}

inline GLuint Shader::getProgram()
{
    return mProgram;
}

#endif
