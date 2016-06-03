#version 330 core
layout (location = 0) out vec3 a_Result;

in vec2 v_TexCoord;

struct DirLight
{
    vec3 direction;     
    vec3 color;
};

uniform vec3 u_ViewPos;
uniform sampler2D u_Position;
uniform sampler2D u_Normal;
uniform sampler2D u_Color;
//uniform sampler2D u_Diffuse;
//uniform sampler2D u_Specular;
uniform sampler2D u_Result;
uniform sampler2D u_Depth;
uniform DirLight u_DirLight;



vec3 calcDirLight(DirLight light, vec3 viewDir, vec3 normal, vec3 diffColor, float specColor)
{
    vec3 lightDir = normalize(-light.direction);
    vec3 reflectDir = reflect(-lightDir, normal);
    
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0);
   
    vec3 ambient = light.color * diffColor;
    vec3 diffuse = light.color * diff * diffColor; 
    vec3 specular = light.color * spec * specColor; 
    
    return ambient + diffuse + specular;
}


void main()
{    
    vec3 position = texture(u_Position, v_TexCoord).rgb;
    vec3 normal = texture(u_Normal, v_TexCoord).rgb;
    vec4 color = texture(u_Color, v_TexCoord);
    vec4 result = texture(u_Result, v_TexCoord);
    float depth = texture(u_Depth, v_TexCoord).r;
//  vec3 diff = texture(u_Diffuse, v_TexCoord).rgb;
//   vec3 spec = texture(u_Specular, v_TexCoord).rgb;

    vec3 viewDir = normalize(u_ViewPos - position);

// vec3 result = calcDirLight(u_DirLight, viewDir, normal, color.rgb, color.a); 

    a_Result = color.rgb;  
}

