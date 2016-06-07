#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;

uniform mat4 u_Proj;
varying vec3 v_ViewRay;
varying vec2 v_TexCoord;


void main()
{
    vec4 pos = vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
    vec2 halfNearSpace = vec2(1.0/u_Proj[0].x, 1.0/u_Proj[1].y);
    v_ViewRay = vec3( -halfNearSpace * pos.xy, 1.0);

    gl_Position = pos;
}
