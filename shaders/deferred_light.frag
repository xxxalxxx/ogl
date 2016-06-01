#version 330 core
layout (location = 3) out vec3 a_Diffuse;
layout (location = 4) out vec3 a_Specular;

struct PointLight
{
    vec3 position;
    
    float quadratic;
    float linear;
    float constant;    
    vec3 color;
};

uniform vec3 u_ViewPos;
uniform vec2 u_ViewportDims;
uniform PointLight light;

uniform sampler2D u_Position;
uniform sampler2D u_Normal;

void main()
{    
    vec2 texCoord = gl_FragCoord.xy;
    texCoord.x /=  800.0;
    texCoord.y /= 600.0;
    vec3 position = texture(u_Position,texCoord).rgb;
    vec3 normal = texture(u_Normal, texCoord).rgb;  
    vec3 viewDir = normalize(u_ViewPos - position); 
    vec3 lightDir = normalize(light.position - position);
    vec3 reflectDir = reflect(-lightDir, normal);
   
    float d = length(light.position - position);
    float attenuation = 1.0/(light.constant + light.linear * d + light.quadratic * d * d);  

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0);
 
    a_Diffuse = light.color * diff * attenuation; 
    a_Specular = light.color * spec * attenuation;
}

