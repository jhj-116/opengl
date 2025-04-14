
#SHADER VERTEX
#version 330 core
layout (location = 0) in vec4 position;
//layout( location = 1) in vec2  texcoord;
out vec2 v_TexCoord;
uniform mat4 u_MVP;

//下面是批量渲染颜色
layout( location = 1) in vec4  v_color;
out vec4 u_Tcolor;

//下面是批量渲染纹理
layout( location = 2) in vec2  texcoords;
out float u_TexureIndex;
out vec2 v_TexCoords;
layout( location = 3) in float  v_TexureIndex;
void main()
{
   gl_Position = u_MVP *position;
   //gl_Position = position;
  // v_TexCoord = texcoord;

  //u_Tcolor=v_color;
  v_TexCoords= texcoords;
  u_TexureIndex=v_TexureIndex;
  

};

#SHADER FRAGMENT
#version 330 core

in vec2 v_TexCoord; //尼玛的v_Texcoord名字还必须与顶点着色器的输出纹理坐标数据名字一样(v_TexCoord)，不然没有纹理坐标数据
out vec4 FragColor;
uniform vec4 u_color;
uniform sampler2D u_Texture;

//下面是批量渲染颜色
in vec4 u_Tcolor; 

////下面是批量渲染纹理
uniform sampler2D u_Textures[2];
in float u_TexureIndex;
in vec2 v_TexCoords;
void main()
{
	//vec4 texcolor = texture(u_Texture, v_TexCoord);
	//texcolor += u_color;
	//FragColor = texcolor;
    //FragColor = u_Tcolor;

	int index=int(u_TexureIndex);
	vec4 texcolor = texture(u_Textures[index], v_TexCoords);
	FragColor = texcolor;

};