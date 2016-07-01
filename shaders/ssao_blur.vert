#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

uniform float u_ScaleSSAO;

void main()
{
    gl_Position = vec4(a_Position, 1.0);
    v_TexCoord = u_ScaleSSAO * a_TexCoord;

}
