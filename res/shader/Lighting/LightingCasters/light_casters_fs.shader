#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 

//这是定向定向光源的light类
//struct Light {
    //vec3 position;/我们可以通过定义光线方向向量而不是位置向量来模拟这样的定向光源。
    //着色器计算基本保持不变，只是这次我们直接使用光源的矢量，而不是使用光源的矢量计算矢量
 //   vec3 direction;
 //   vec3 ambient;
 //   vec3 diffuse;
//    vec3 specular;
//};

//这是点光灯的light类
//实施衰减
//为了实现衰减，我们需要在片段着色器中增加 3 个值：即方程的常数项、线性项和二次项。这些最好存储在我们之前定义的 Light 结构体中。
//struct Light {
 //   vec3 position;  
  
 //   vec3 ambient;
 //   vec3 diffuse;
 //  vec3 specular;
	
//    float constant;
//    float linear;
//    float quadratic;
//}; 

//这是聚光灯的light类
struct Light {
    vec3  position;//聚光灯的位置
    vec3  direction;//聚光灯的方向向量
    float cutOff;//聚光灯的截止角度
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;  
};   

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
     vec3 lightDir = normalize(light.position - FragPos);//我们使用的光照计算希望光源方向是从片段到光源的方向
    //人们通常更喜欢将定向光源指定为指向光源的全局方向。因此，我们必须对全局光线方向向量求反以切换其方向;现在是指向光源的方向向量
   // vec3 lightDir = normalize(-light.direction);  

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  
        
     // attenuation 点光灯
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); //利用公式计算点光源的光照强度随着距离进行衰弱值   

    ambient  *= attenuation;  
    diffuse   *= attenuation;
    specular *= attenuation;   
    //您可以看到，现在只有前面的容器被照亮，最近的容器是最亮的。后面的容器根本没有点亮，因为它们离光源太远了。

    //聚光灯
    // spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff); //创建边缘平滑的聚光灯的效果，我们想要模拟具有内锥体和外锥体的聚光灯
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);//我=我 -C/ε 
    diffuse  *= intensity;
    specular *= intensity;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
  
    
} 