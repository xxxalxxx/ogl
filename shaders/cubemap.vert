#version 330 core
layout (location = 0) in vec3 a_Position;

uniform mat4 u_WVP;

varying vec3 v_TexCoord;

void main()
{
    v_TexCoord = vec3(-a_Position.x, a_Position.yz);
    
    gl_Position = (u_WVP *  vec4(a_Position, 1.0)).xyww;
}
