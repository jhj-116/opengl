#version 330 core
out vec4 FragColor;

in vec3 textureDir; //方向向量

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, textureDir);
}