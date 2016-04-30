#include "Quad.h"
Quad::Quad()
{

}


Quad::~Quad()
{
    unload();
}


void Quad::init()
{
    initShaders();
    initBuffers();
    initTextures();
    
    mModel = glm::mat4();
}

void Quad::initBuffers()
{

     GLfloat vertices[] = {
        // Positions           // Texture Coords
         1.0f,  1.0f, 0.0f,     1.0f, 1.0f, // Top Right
         1.0f, -1.0f, 0.0f,    1.0f, 0.0f, // Bottom Right
        -1.0f, -1.0f, 0.0f,    0.0f, 0.0f, // Bottom Left
        -1.0f,  1.0f, 0.0f,     0.0f, 1.0f  // Top Left 
    };
    GLuint indices[] = {
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };
  
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);

    glBindVertexArray(mVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLsizei stride = 5*sizeof(GLfloat);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    
}

void Quad::initShaders()
{   

}

void Quad::initTextures()
{
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);

    int w, h;
    unsigned char* img = SOIL_load_image("res/img.jpg", &w, &h, 0, SOIL_LOAD_RGB);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
    glGenerateMipmap(GL_TEXTURE_2D);

    SOIL_free_image_data(img);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Quad::draw(const glm::mat4& viewProj, GLuint program, float dt)
{
   
    glm::mat4 MVP = viewProj * mModel;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    GLuint MVPHandle = glGetUniformLocation(program, "u_MVP");
    glUniformMatrix4fv(MVPHandle, 1, GL_FALSE, glm::value_ptr(MVP) );
    glUniform1i(glGetUniformLocation(program, "u_Sampler"), 0);

    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Quad::unload()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
    glDeleteBuffers(1, &mEBO);
}



