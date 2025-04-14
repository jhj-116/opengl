#version 330 core
//out vec4 FragColor;
//in vec2 TexCoords;
//uniform sampler2D texture1;
//void main()
//{    
 //   FragColor = texture(texture1, TexCoords);
//}

//反射
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{    
    //反射
    //vec3 I = normalize(Position - cameraPos);
    //vec3 R = reflect(I, normalize(Normal));

    //折射
    float ratio = 1.00 / 1.52;//光线/视野光线从空气到玻璃（如果我们假设物体是由玻璃制成的），
    //因此比率变为1.001.52=0.658
    vec3 I = normalize(Position - cameraPos);
    vec3 R = refract(I, normalize(Normal), ratio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);

   
}