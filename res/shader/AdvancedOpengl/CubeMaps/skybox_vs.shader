#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 textureDir;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    /*
    立方体以原点 （0,0,0） 为中心时，它的每个位置向量也是从原点开始的方向向量。
    这个方向向量正是我们在该特定立方体位置获得相应纹理值所需要的。
    因此，我们只需要提供位置向量(aPos)，不需要纹理坐标
    */
    textureDir = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;//(设置深度测试值1)
   // gl_Position = vec4(pos.x,pos.y,1.0,1.0);
}  