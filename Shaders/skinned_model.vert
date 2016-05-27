#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_Bitangent;
layout (location = 5) in ivec4 a_BoneIndices;
layout (location = 6) in vec4 a_Weights;

out vec2 v_TexCoords;

#define MAX_BONES 100

uniform mat4 u_MVP;
uniform mat4 u_Bones[MAX_BONES];


void main()
{
    mat4 boneTransform = u_Bones[a_BoneIndices.x] * a_Weights.x;
    boneTransform += u_Bones[a_BoneIndices.y] * a_Weights.y;
    boneTransform += u_Bones[a_BoneIndices.z] * a_Weights.z;  
    boneTransform += u_Bones[a_BoneIndices.w] * a_Weights.w;

    gl_Position = u_MVP * boneTransform * vec4(a_Position, 1.0);
    v_TexCoords = a_TexCoords;
}

