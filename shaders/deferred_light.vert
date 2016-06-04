#version 330 core
layout (location = 0) in vec3 a_Position;

uniform mat4 u_WVP;
uniform mat4 u_View;
uniform mat4 u_World;
uniform mat4 u_Proj;

varying vec3 v_LightPosW;
varying mat4 invView;
varying vec4 v_ViewRay;
varying vec3 v_TexCoord;

void main()
{
    vec4 pos = vec4(a_Position, 1.0);
    v_LightPosW = u_World[3].xyz; 

    vec4 result = u_WVP * pos;

    invView = inverse(u_View);
    
    vec2 halfNearPlane = vec2(u_Proj[0].x, u_Proj[1].y);
    v_TexCoord = vec3(0.5 * (result.xy + result.w), result.w);
    v_ViewRay = vec4(result.xy / halfNearPlane, -result.w, result.w);

    gl_Position = result;
}
