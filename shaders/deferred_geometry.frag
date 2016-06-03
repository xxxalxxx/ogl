#version 330 core
layout (location = 0) out vec3 a_Position;
layout (location = 1) out vec3 a_Normal;
layout (location = 2) out vec4 a_Color;
//layout (location = 3) out vec4 a_Result;
//layout (location = 3) out vec4 a_Depth;
in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoord;

#define MAX_DIFFUSE_TEX 8
#define MAX_SPECULAR_TEX 8

uniform sampler2D u_TextureDiffuse[MAX_DIFFUSE_TEX];
uniform sampler2D u_TextureSpecular[MAX_SPECULAR_TEX];

void main()
{    
    a_Position = v_Position;
    a_Normal = v_Normal;
    a_Color.rgb = texture(u_TextureDiffuse[0], v_TexCoord).rgb;
    a_Color.a = texture(u_TextureSpecular[0], v_TexCoord).r; 
   // a_Result = a_Color;
  //  a_Depth = vec4(vec3(gl_FragCoord.z),1.0);
}

