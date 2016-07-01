#include "cone.h"

Cone::Cone(): mNumIndices(0) , mVAO(0), mVBO(0), mEBO(0)
{

}

Cone::~Cone()
{

}

bool Cone::init(size_t numSlices, float height /* = 1.0f */, float bottomCapRadius/* = 1.0f */)
{
    if(!numSlices || !height || !bottomCapRadius) return false;
    float h = fabs(height);
    float r = fabs(bottomCapRadius);
    float dt = 2.0f*M_PI/((float)numSlices);

    std::vector<glm::vec3> vertices;
    std::vector<GLuint> indices;

    vertices.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    for(size_t i=0;i<numSlices;++i)
    {
        float angle = i * dt;
        vertices.push_back(glm::vec3(r*cosf(angle), -h, r*sinf(angle)));
    }
    //first triangle
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back((GLuint)numSlices-1);
    

    for(size_t i=2;i<numSlices;++i)
    {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i-1);
    }

    //first triangle again to close cone
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back((GLuint)numSlices-1);

    //bottom cap fan in clockwise order
    for(size_t i=2;i<numSlices-1;++i)
    {
        indices.push_back(1);
        indices.push_back(i);
        indices.push_back(i+1);
    }

    mNumIndices = indices.size();
    initBuffers(vertices, indices);

    return true;
}

void Cone::initBuffers(const std::vector<glm::vec3>& vertices, const std::vector<GLuint>& indices)
{
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);

    glBindVertexArray(mVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  sizeof(glm::vec3), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}


void Cone::draw()
{
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}

void Cone::unload()
{

}
