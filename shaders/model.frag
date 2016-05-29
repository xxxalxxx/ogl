#version 330 core
in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoord;

#define MAX_DIFFUSE_TEX 8
#define MAX_SPOT_LIGHTS 8
#define MAX_POINT_LIGHTS 8



struct DirLight
{
    vec3 direction;    

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;
    
    float a0;
    float a1;
    float a2;    

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

};

struct SpotLight
{
    vec3 direction;
    vec3 position;

    float cutoff;
    float cutoffStart;
    float exponent;  
    
    float a0;
    float a1;
    float a2;    
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform sampler2D u_TextureDiffuse[MAX_DIFFUSE_TEX];
uniform DirLight u_DirLight;
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform SpotLight u_SpotLights[MAX_SPOT_LIGHTS];

vec3 calcDirLight();
vec3 calcPointLight();
vec3 calcSpotLight();

void main()
{    
    gl_FragColor = vec4(texture(u_TextureDiffuse[0], v_TexCoord).rgb, 1.0);
}


