#version 330 core
in vec2 TexCoords;
out vec4 color;

#define MAX_DIFFUSE_TEX 8

uniform sampler2D texture_diffuse[MAX_DIFFUSE_TEX];

void main()
{    
    color = texture(texture_diffuse[0], TexCoords);
}
