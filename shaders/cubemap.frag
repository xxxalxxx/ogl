#version 330 core

in vec3 v_TexCoord;

uniform samplerCube u_Sampler;

void main()
{    
    gl_FragColor = vec4(texture(u_Sampler, v_TexCoord).rgb, 1.0);
}
  
