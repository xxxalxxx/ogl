#version 330 core
in vec2 v_TexCoord;
in vec3 v_Pos;
uniform sampler2D u_Sampler;

void main()
{
    gl_FragColor = vec4(texture(u_Sampler, v_TexCoord).xyz, 1.0);
}

