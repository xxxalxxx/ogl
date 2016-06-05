#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;

uniform mat4 u_WVP;
uniform mat4 u_Proj;

varying vec2 v_ViewRay;
varying vec2 v_TexCoord;

void main()
{
    vec4 pos = u_WVP * vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
    v_ViewRay = vec2(pos.x/u_Proj[0].x, pos.y/u_Proj[1].y);
    gl_Position = pos;

}
