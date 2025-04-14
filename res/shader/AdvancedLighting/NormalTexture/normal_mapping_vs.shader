#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));   
    vs_out.TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));//计算法线矩阵
    //下面计算切线与双切线
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N)); //TBN切线矩阵(切线，双切线和法线向量)   
    vs_out.TangentLightPos = TBN * lightPos;        //利用TBN将世界坐标下光照位置，转到切线空间下的位置
    vs_out.TangentViewPos  = TBN * viewPos;         //利用TBN将世界坐标下viewPos，转到切线空间下的位置
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;  //利用TBN将世界坐标下Cube的位置，转到切线空间下的位置
      
    //用来计算光照镜面反射，漫反射公式
    //这么做原因可以看NormalTextureApplication.cpp注释讲解
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}