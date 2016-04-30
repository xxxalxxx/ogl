#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;
out vec3 v_Pos;
uniform mat4 u_MVP;

void main()
{
    vec4 position = u_MVP * vec4(a_Position,1.0);
    gl_Position = position;
    v_TexCoord = a_TexCoord;
    v_Pos = a_Position;
}
