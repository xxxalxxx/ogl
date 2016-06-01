#include "sphere.h"

Sphere::Sphere(): mVAO(0), mEBO(0), mVBO(0)
{

}

Sphere::~Sphere()
{

}

bool Sphere::init(size_t numStacks, size_t numSlices, float radius)
{
    std::vector<glm::vec3> vertices;
    std::vector<GLuint> indices;
   
    float phiStep = M_PI/numStacks;

    size_t numRings = numStacks-1;

    for(size_t i = 1; i <= numRings; ++i)
    {
        float phi = i*phiStep;

        // vertices of ring
        float thetaStep = 2.0f*M_PI/numSlices;
        for(size_t j = 0; j <= numSlices; ++j)
        {
            float theta = j*thetaStep;

            glm::vec3 v;

            v.x = radius*sinf(phi)*cosf(theta);
            v.y = radius*cosf(phi);
            v.z = radius*sinf(phi)*sinf(theta);

            vertices.push_back(v);
        }
    }

    vertices.push_back( glm::vec3(0.0f, -radius, 0.0f) );
    vertices.push_back( glm::vec3(0.0f, radius, 0.0f) );

    size_t northPoleIndex = vertices.size()-1;
    size_t southPoleIndex = vertices.size()-2;

    size_t numRingVertices = numSlices+1;

    for(size_t i = 0; i < numStacks-2; ++i)
    {
        for(size_t j = 0; j < numSlices; ++j)
        {
            indices.push_back(i*numRingVertices + j);
            indices.push_back(i*numRingVertices + j+1);
            indices.push_back((i+1)*numRingVertices + j);

            indices.push_back((i+1)*numRingVertices + j);
            indices.push_back(i*numRingVertices + j+1);
            indices.push_back((i+1)*numRingVertices + j+1);
        }
    }

    for(size_t i = 0; i < numSlices; ++i)
    {
        indices.push_back(northPoleIndex);
        indices.push_back(i+1);
        indices.push_back(i);
    }

    size_t baseIndex = (numRings-1)*numRingVertices;
    for(size_t i = 0; i < numSlices; ++i)
    {
        indices.push_back( (GLuint) southPoleIndex);
        indices.push_back( (GLuint) baseIndex+i);
        indices.push_back( (GLuint) baseIndex+i+1);
    }

    mNumIndices = indices.size();
    initBuffers(vertices, indices);
    return true;
}

void Sphere::initBuffers(const std::vector<glm::vec3>& vertices, const std::vector<GLuint>& indices)
{
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);

    glBindVertexArray(mVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

    GLsizei stride = sizeof(glm::vec3);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);


}

void Sphere::draw()
{
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}


