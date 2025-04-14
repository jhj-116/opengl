#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_specular1;
float near=0.1f;
float far=100.0f;
float LinearizeDepth(float depth)
{
    float z=depth*2-1;
    return (2.0f*near*far)/(far+near-z*(far=near));
}

void main()
{    
    //FragColor = texture(texture_diffuse1, TexCoords);
   //FragColor = texture(texture_specular1, TexCoords);
   float depth=LinearizeDepth(gl_FragCoord.z);
   FragColor = vec4(vec3(depth), 1.0f);
}