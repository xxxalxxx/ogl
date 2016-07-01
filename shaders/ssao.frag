#version 330 core
layout (location = 0) out float a_SSAO;

in vec2 v_TexCoord;
in vec3 v_ViewRay;
in mat4 v_Proj;

#define KERNEL_SIZE 32
#define NOISE_SIZE 4
#define NEAR 0.1
#define FAR 1000.0

uniform vec3 u_Kernel[KERNEL_SIZE];
uniform float u_Radius;
uniform float u_ScaleSSAO;
uniform mat4 u_Proj;

uniform sampler2D u_Normal;
uniform sampler2D u_Depth;
uniform sampler2D u_Noise;


vec3 getNormalW(in vec2 packedNormal)
{
    float nz = sqrt(1.0 - dot(packedNormal.xy, packedNormal.xy));
    return vec3(packedNormal.xy, nz);
}

float getLinearDepth(float depth)
{
    return NEAR * FAR / (depth * (FAR - NEAR) - FAR);
}

void main()
{
    vec2 noiseScale = vec2(textureSize(u_Normal, 0))/float(NOISE_SIZE);
    float depth = texture(u_Depth, v_TexCoord * u_ScaleSSAO).x;
    vec2 packedNormal = texture(u_Normal, v_TexCoord * u_ScaleSSAO).xy;
    vec3 randVec = texture(u_Noise, v_TexCoord  * noiseScale).xyz;

    vec3 normal = getNormalW(packedNormal);
    vec3 tangent = normalize(randVec - normal * dot(randVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tbn = mat3(tangent, bitangent, normal); 
   
    float viewZ = getLinearDepth(depth);
    vec3 posV = viewZ * v_ViewRay.xyz;

    float AO = 0.0;

    for(int i=0;i<KERNEL_SIZE;++i)
    {
        vec3 sample = tbn * u_Kernel[i];
        sample = u_Radius * sample + posV;
        vec4 offset = vec4(sample, 1.0);
        offset = u_Proj * offset;
        offset.xy /= offset.w;
        offset.xy  = 0.5 * offset.xy + 0.5;        

        float sampleDepth = texture(u_Depth, offset.xy * u_ScaleSSAO).x;
        sampleDepth = getLinearDepth(sampleDepth);    

        float radiusCheck = abs(posV.z - sampleDepth) < u_Radius ? 1.0 : 0.0;

        //sample z must be less, then sampleDepth, but since we're comparing two negatives, we need to flip a bool expression
        AO += (sampleDepth > sample.z ? 1.0 : 0.0) * radiusCheck;
    }
    
    a_SSAO = 1.0 - AO/float(KERNEL_SIZE);
}
