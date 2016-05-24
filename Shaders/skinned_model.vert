#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in ivec4 boneIndices;
layout (location = 4) in vec4 weights;

out vec2 TexCoords;

#define MAX_BONES 100

uniform mat4 u_MVP;
uniform mat4 u_Bones[MAX_BONES];


void main()
{
    mat4 boneTransform = u_Bones[boneIndices.x] * weights.x;
    boneTransform += u_Bones[boneIndices.y] * weights.y;
    boneTransform += u_Bones[boneIndices.z] * weights.z;  
    boneTransform += u_Bones[boneIndices.w] * weights.w;

    gl_Position = u_MVP * boneTransform * vec4(position, 1.0);
    TexCoords = texCoords;
}

