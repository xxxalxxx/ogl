#version 330 core
layout (location = 0) out vec3 a_Result;

in vec2 v_TexCoord;

#define MAX_LAYERS 5

uniform sampler2D u_Sampler;
uniform int u_NumLayers;
uniform sampler2D u_Layers[MAX_LAYERS];
uniform float u_Steps[MAX_LAYERS];

#define handleLayer(i, h) if(u_NumLayers > i && h < u_Steps[i]) {return texture(u_Layers[i], texCoord).rgb;}

vec3 getLayerColor(in vec2 texCoord, float height)
{
    //loop needs to be unrolled
    handleLayer(0, height);
    handleLayer(1, height);
    handleLayer(2, height);
    handleLayer(3, height);
    handleLayer(4, height);

    return vec3(1.0);
}


void main()
{
    ivec2 texSize = ivec2(255, 255); // textureSize(u_Sampler, 0);
    ivec2 windowSize = ivec2(800, 600);
    
    vec2 texCoord = v_TexCoord.yx;
    float height = texture(u_Sampler, texCoord*vec2(600.0/255.0, 800.0/255.0)).r; 

    //texCoord.y *= -1.0;
   // a_Result = vec3(height);
    a_Result = getLayerColor(texCoord * 10 , height); 
}

