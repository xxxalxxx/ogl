#version 330 core
layout (location = 0) out vec3 a_Result;

struct PointLight
{
    vec3 position;
    
    float quadratic;
    float linear;
    float constant;    
    vec3 color;
};

uniform vec3 u_ViewPos;
uniform PointLight light;

uniform sampler2D u_Position;
uniform sampler2D u_Normal;
uniform sampler2D u_Color;

void main()
{    
    vec2 dims = vec2(800.0, 600.0);
    vec2 texCoord = gl_FragCoord.xy/dims;
    
    vec3 position = texture(u_Position, texCoord).rgb;
    vec3 normalT = texture(u_Normal, texCoord).rgb;
    float nz = sqrt(1.0 - dot(normalT.xy, normalT.xy));
    vec3 normal = vec3(normalT.xy, nz);
    vec4 color = texture(u_Color, texCoord);
  
    vec3 viewDir = normalize(u_ViewPos - position); 
    vec3 lightDir = normalize(light.position - position);
    vec3 reflectDir = reflect(-lightDir, normal);
   
    float d = length(light.position - position);
    float attenuation = light.constant + light.linear * d + light.quadratic * d * d;  

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0);
    vec3 diffuse = light.color * color.rgb * diff;
    vec3 specular = light.color * color.a * spec;

    a_Result = (diffuse + specular)/attenuation;

    
}

