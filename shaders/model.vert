#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_Bitangent;

uniform mat4 u_WVP;
uniform mat4 u_World;

out vec3 v_Normal;
out vec3 v_Position;
out vec2 v_TexCoord;



void main()
{
    vec4 pos = vec4(a_Position, 1.0);

    v_Position = vec3(u_World * pos);
    v_Normal = a_Normal;
    v_TexCoord = a_TexCoord;

    gl_Position = u_WVP * pos;
}




