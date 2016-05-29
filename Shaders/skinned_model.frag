#version 330 core
in vec2 v_TexCoord;

#define MAX_DIFFUSE_TEX 8

uniform sampler2D u_TextureDiffuse[MAX_DIFFUSE_TEX];

void main()
{    
    gl_FragColor = vec4(texture(u_TextureDiffuse[0], v_TexCoord).rgb, 1.0);
}


