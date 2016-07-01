#version 330 core
layout (location = 0) in vec3 a_Position;


out vec2 v_TexCoord;

uniform mat4 u_WVP;

void main()
{
    gl_Position = u_WVP * vec4(a_Position, 1.0);
    v_TexCoord = a_Position.xz + 0.5;

}
