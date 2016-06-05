#version 330 core
layout (location = 0) in vec3 a_Position;

uniform mat4 u_WVP;

uniform mat4 u_World;
uniform mat4 u_Proj;

varying vec3 v_LightPosW;
varying mat3 v_CamWorld;
varying vec4 v_ViewRay;
varying vec3 v_TexCoord;
varying vec3 v_ScaleAndTranslateProjs;
varying mat4 v_Proj;
void main()
{
    vec4 pos = vec4(a_Position, 1.0);
    v_LightPosW = u_World[3].xyz; 

    vec4 result = u_WVP * pos;
    v_Proj = u_Proj;    
    v_TexCoord = vec3(0.5 * (result.xy + result.w), result.w);
    v_ScaleAndTranslateProjs = vec3(u_Proj[2][2], u_Proj[3][2], result.w);
    v_ViewRay = vec4(result.x/u_Proj[0].x, result.y/u_Proj[1].y, -result.w, result.w);

    gl_Position = result;
}
