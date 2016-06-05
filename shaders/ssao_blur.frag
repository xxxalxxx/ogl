#version 330 core
layout (location = 0) out float a_BlurredSSAO;

in vec2 v_TexCoord;

uniform sampler2D u_SSAO;

#define BLUR_SIZE 4

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(u_SSAO, 0));
    float result = 0.0;

    for (int x = 0; x < BLUR_SIZE; ++x) 
    {
        for (int y = 0; y < BLUR_SIZE; ++y) 
        {
            vec2 offset = (vec2(-2.0) + vec2(float(x), float(y))) * texelSize;
            result += texture(u_SSAO, v_TexCoord + offset).r;
        }
    }
    a_BlurredSSAO = result / float(BLUR_SIZE * BLUR_SIZE);
 
 //  a_BlurredSSAO = texture(u_SSAO, v_TexCoord).r;
}
