#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_Bitangent;

out vec2 v_TexCoord;

uniform mat4 u_WVP;

void main()
{
    gl_Position = u_WVP * vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
}




