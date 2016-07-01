#version 330 core
layout (location = 0) in vec3 a_Position;

out vec2 v_TexCoord;

void main()
{
    gl_Position = vec4(a_Position, 1.0);
    v_TexCoord = 0.5 * a_Position.xy + 0.5;
}
