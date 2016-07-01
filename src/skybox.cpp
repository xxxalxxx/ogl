#include "skybox.h"


Skybox::Skybox()
{

}

Skybox::~Skybox()
{

}

void Skybox::init()
{
    GLfloat vertices[] = {
        -1.0f,  1.0f,  1.0f, //top left far
         1.0f,  1.0f,  1.0f, //top right far
        -1.0f, -1.0f,  1.0f, //bottom left far
         1.0f, -1.0f,  1.0f, //bottom right far   

        -1.0f,  1.0f, -1.0f, //top left close
         1.0f,  1.0f, -1.0f, //top right close
        -1.0f, -1.0f, -1.0f, //bottom left close
         1.0f, -1.0f, -1.0f, //bottom right close

    };

    GLuint indices[] = {
        //bottom -> back -> top + degenerate to restart
        2, 3, 6, 7,
        4, 5, 0, 1, 1,
        //right -> front -> left
        5, 7, 1, 3,
        0, 2, 4, 6
     };
    
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);

    glBindVertexArray(mVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}


void Skybox::draw()
{
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLE_STRIP, 17, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Skybox::unload()
{

}







