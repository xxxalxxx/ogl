#version 330 core
//layout (location = 0) out vec3 a_Diffuse;
//layout (location = 1) out vec3 a_Specular;

in vec2 v_TexCoord;
uniform sampler2D u_Depth;
uniform sampler2D u_Position;
uniform sampler2D u_Normal;

void main()
{    
    vec2 dims = vec2(800,600);
    vec2 t = gl_FragCoord.xy/dims;
    float d = texture(u_Depth,t).r;
 //  vec3 p = texture(u_Position, t).xyz;
 //   vec3 n = texture(u_Normal, t).xyz;
    gl_FragColor = vec4(d,d,d,1.0);
// a_Diffuse =  vec3(0.0,1.0,0.0);
   // a_Specular = n;
}

