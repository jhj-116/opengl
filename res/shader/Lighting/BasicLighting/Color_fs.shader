#version 330 core

in vec3 FragPos;  
  
uniform vec3 lightPos;

out vec4 FragColor;
in vec3 u_normal;
uniform vec3 objectColor;
uniform vec3 lightColor;

//镜面反射
uniform vec3 viewPos;
void main()
{
     // ambient 环境
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 漫反射
	vec3 norm = normalize(u_normal);
    vec3 lightDir = normalize(lightPos - FragPos);//算出光照的入射方向(向量)
    float diff = max(dot(norm, lightDir), 0.0);//这部分是漫反射，入射光线与顶点平面法线量的夹角有关，漫反射强度跟夹角的点积大小相关
    vec3 diffuse = diff * lightColor;//这上面是环境光照
            

     //镜面反射
    float specularStrength = 0.5;//镜面反射强度值
    vec3 viewDir=normalize(viewPos-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  //这表示光线方向的反向，即从表面指向光源的方向。反向是因为反射公式reflect需要光线的入射方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);//反射方向 reflectDir 表示光线经过表面反射后朝向的方向

    //我们首先计算视图方向和反射方向之间的点积（并确保它不是负数），然后将其提高到 的幂。
    //该值是高光的光泽度值。对象的光泽度值越高，它就越能正确地反射光线，而不是将其散射到周围，因此高光就越小。
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse+specular) * objectColor;
    FragColor = vec4(result, 1.0);

   
}