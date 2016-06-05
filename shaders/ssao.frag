#version 330 core
layout (location = 0) out float a_SSAO;

in vec2 v_TexCoord;
in vec2 v_ViewRay;
in mat4 v_Proj;

#define KERNEL_SIZE 32

uniform vec3 u_Kernel[KERNEL_SIZE];
uniform float u_Radius;
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
    return u_Proj[3][2]/(2.0 * depth - 1.0 - u_Proj[2][2]);
}

const vec2 noiseScale = vec2(800.0, 600.0)/4.0;

void main()
{
    float depth = texture(u_Depth, v_TexCoord).x;
    vec2 packedNormal = texture(u_Normal, v_TexCoord).xy;
    vec3 randVec = texture(u_Noise, v_TexCoord * noiseScale).xyz;

    vec3 normal = getNormalW(packedNormal);
    vec3 tangent = normalize(randVec - normal * dot(randVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tbn = mat3(tangent, bitangent, normal); 
    
    float viewZ = getLinearDepth(depth);
    vec3 posV = vec3(v_ViewRay, viewZ);
    posV.xy *= viewZ;
    
    float AO = 0.0;

    for(int i=0;i<KERNEL_SIZE;++i)
    {
        vec3 sample = tbn * u_Kernel[i];
        sample = u_Radius * sample + posV;
      //  vec3 sample = u_Kernel[i] + posV;
        vec4 offset = vec4(sample, 1.0);
        offset = u_Proj * offset;
        offset.xy /= offset.w;
        offset.xy  = 0.5 * offset.xy + 0.5;        

        float sampleDepth = texture(u_Depth, offset.xy).x;
        sampleDepth = getLinearDepth(sampleDepth);    

        if(abs(posV.z - sampleDepth) < u_Radius)
        {
            AO += step(sampleDepth, sample.z);
        }

    }

    a_SSAO = pow(1.0 - AO/float(KERNEL_SIZE), 1.0);


}
