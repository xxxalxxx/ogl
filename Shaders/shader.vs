#version 330 core
layout (location = 0) in vec3 position;
// layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;

uniform mat4 u_MVP;


void main()
{
    gl_Position = u_MVP * vec4(position, 1.0f);
    TexCoords = texCoords;
}
