#version 330 core
layout (location = 0) out vec3 a_Result;


in vec3 v_LightDir;
in vec3 v_LightPosW;

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



vec3 getSpotLight(in SpotLight light, in vec4 color, in vec3 position, in vec3 normal, in vec3 viewPos)
{
   // light.direction = v_LightDir;
   // light.position = v_LightPosW;
    vec3 viewDir = normalize(viewPos - position); 
    vec3 lightDir = normalize(light.position - position); 
    vec3 reflectDir = reflect(-lightDir, normal);  
    
    float theta = dot(lightDir, normalize(-light.direction)); 
    float dTheta = light.cutoffStart - light.cutoff;
    float intensity = clamp((theta - light.cutoff) / dTheta, 0.0, 1.0);
    
    float d = length(light.position - position);
    float attenuation = light.constant + light.linear * d + light.quadratic * d * d;  

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0);

    vec3 diffuse = light.color * color.rgb * diff; 
    vec3 specular = light.color * color.a * spec;


    return intensity * ( diffuse + specular) / attenuation;
}



in vec4 v_ViewRay;
in vec3 v_TexCoord;

uniform mat4 u_Proj;
uniform vec3 u_ViewPos;
uniform SpotLight light;
uniform mat3 u_CamWorld;

uniform sampler2D u_Normal;
uniform sampler2D u_Color;
uniform sampler2D u_Depth;

#define NEAR 0.1
#define FAR 1000.0

vec3 getPosW(float depth, in vec3 viewRay, in mat3 camWorld, in vec3 viewPos)
{
    float viewZ = NEAR * FAR / (FAR - depth * (FAR + NEAR));
    vec3 posV = viewRay * viewZ;
    return camWorld * posV + viewPos;
}

vec3 getNormalW(in vec2 packedNormal)
{
    float nz = sqrt(1.0 - dot(packedNormal.xy, packedNormal.xy));
    return vec3(packedNormal.xy, nz);
}

void main()
{    
    vec2 texCoord = v_TexCoord.xy/v_TexCoord.z;
    
    vec4 color = texture(u_Color, texCoord);
    vec2 packedNormal = texture(u_Normal, texCoord).xy;
    float depth = texture(u_Depth, texCoord).x;
   
    vec3 viewRay = v_ViewRay.xyz/v_ViewRay.w;

    vec3 position = getPosW(depth, viewRay, u_CamWorld, u_ViewPos);
    vec3 normal = getNormalW(packedNormal);

    a_Result = getSpotLight(light, color, position, normal, u_ViewPos);
}


