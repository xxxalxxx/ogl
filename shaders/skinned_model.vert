#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_Bitangent;
layout (location = 5) in ivec4 a_BoneIndices;
layout (location = 6) in vec4 a_Weights;


out vec3 v_Position;
out vec3 v_Normal;
out vec2 v_TexCoord;

#define MAX_BONES 100

uniform mat4 u_World;
uniform mat4 u_WVP;
uniform mat4 u_Bones[MAX_BONES];


void main()
{
    mat4 boneTransform = u_Bones[a_BoneIndices.x] * a_Weights.x;
    boneTransform += u_Bones[a_BoneIndices.y] * a_Weights.y;
    boneTransform += u_Bones[a_BoneIndices.z] * a_Weights.z;  
    boneTransform += u_Bones[a_BoneIndices.w] * a_Weights.w;

    vec4 pos = vec4(a_Position, 1.0);
    mat4 boneWorld = u_World * boneTransform;
    v_Position = vec3(boneWorld * pos);
    v_Normal = vec3(boneWorld * vec4(a_Normal, 0.0));
    v_TexCoord = a_TexCoord;

    gl_Position = u_WVP * boneTransform * pos;

}
