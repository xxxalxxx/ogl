#version 330 core
in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoord;


struct DirLight
{
    vec3 direction;    
    
    vec3 color;

};

struct PointLight
{
    vec3 position;
    
    float quadratic;
    float linear;
    float constant;    
    vec3 color;
};

struct SpotLight
{
    vec3 direction;
    vec3 position;

    float cutoff;
    float cutoffStart;
    float exponent;  
    
    float quadratic;
    float linear;
    float constant;    

    vec3 color;
};


vec3 calcDirLight(DirLight light, vec3 viewDir, vec3 diffColor, vec3 specColor)
{
    vec3 lightDir = normalize(-light.direction);
    vec3 reflectDir = reflect(-lightDir, v_Normal);
    
    float diff = max(dot(v_Normal, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0);
   
    vec3 ambient = light.color * diffColor;
    vec3 diffuse = light.color * diff * diffColor; 
    vec3 specular = light.color * spec * specColor;
  
    
    return ambient + diffuse + specular;
}


vec3 calcPointLight(PointLight light, vec3 viewDir, vec3 diffColor, vec3 specColor)
{
    vec3 lightDir = normalize(light.position - v_Position);
    vec3 reflectDir = reflect(-lightDir, v_Normal);
   
    float d = length(light.position - v_Position);
    float attenuation = light.constant + light.linear * d + light.quadratic * d * d;  

    float diff = max(dot(v_Normal, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0);
 
    vec3 ambient = light.color * diffColor;
    vec3 diffuse = light.color * diff * diffColor; 
    vec3 specular = light.color * spec * specColor;
  
    return (ambient + diffuse + specular)/attenuation; 
}

vec3 calcSpotLight(SpotLight light, vec3 viewDir, vec3 diffColor, vec3 specColor)
{
     
    vec3 lightDir = normalize(light.position - v_Position); 
    vec3 reflectDir = reflect(-lightDir, v_Normal);  
    
    float theta = dot(lightDir, normalize(-light.direction)); 
    float dTheta = light.cutoffStart - light.cutoff;
    float intensity = clamp((theta - light.cutoff) / dTheta, 0.0, 1.0);
    
    float d = length(light.position - v_Position);
    float attenuation = light.constant + light.linear * d + light.quadratic * d * d;  

    float diff = max(dot(v_Normal, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0);

    vec3 ambient = light.color * diffColor;
    vec3 diffuse = light.color * diff * diffColor; 
    vec3 specular = light.color * spec * specColor;

    return (ambient + intensity * ( diffuse + specular))/attenuation;
} 

#define MAX_DIFFUSE_TEX 8
#define MAX_SPECULAR_TEX 8
#define MAX_SPOT_LIGHTS 8
#define MAX_POINT_LIGHTS 8

uniform vec3 u_ViewPos;

uniform sampler2D u_TextureDiffuse[MAX_DIFFUSE_TEX];
uniform sampler2D u_TextureSpecular[MAX_SPECULAR_TEX];

uniform DirLight u_DirLight;
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform SpotLight u_SpotLights[MAX_SPOT_LIGHTS];


void main()
{    
    vec3 diffColor = texture(u_TextureDiffuse[0], v_TexCoord).rgb;
    vec3 specColor = texture(u_TextureSpecular[0], v_TexCoord).rgb;

    vec3 viewDir = normalize(u_ViewPos - v_Position);

    vec3 result = calcDirLight(u_DirLight, viewDir, diffColor, specColor);
    
    for(int i=0;i<MAX_SPOT_LIGHTS;++i)
    {
        result += calcSpotLight(u_SpotLights[i], viewDir, diffColor, specColor);
    }

    for(int i=0;i<MAX_POINT_LIGHTS;++i)
    {
        result += calcPointLight(u_PointLights[i], viewDir, diffColor, specColor);
    }    

    gl_FragColor = vec4(result, 1.0);
}

