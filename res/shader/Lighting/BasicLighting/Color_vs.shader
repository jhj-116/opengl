#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 v_normal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 u_normal;
void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	u_normal=v_normal;
	FragPos = vec3(model * vec4(aPos, 1.0));
	//model * vec4(aPos, 1.0)？为什么？因为顶点位置属性与模型矩阵（而不是视图和投影矩阵）相乘，以将其转换为世界空间坐标，
	//模型坐标系：
	//这是对象的局部坐标系，顶点的位置是相对于该对象的原点定义的
}