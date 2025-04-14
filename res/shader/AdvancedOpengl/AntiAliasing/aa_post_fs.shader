#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
uniform  sampler2D screenTexture;
void main()
{
	vec3 result=texture(screenTexture,TexCoords).rgb;
	float grayscale = 0.2126 * result.r + 0.7152 * result.g + 0.0722 * result.b;
	FragColor=vec4(vec3(grayscale),1.0);

	//FragColor=texture(screenTexture,TexCoords);
}



