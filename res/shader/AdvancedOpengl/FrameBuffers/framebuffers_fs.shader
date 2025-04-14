#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

float sigma=0.8;
float blurSize=1.0f;
float gaussian(vec2 coord)
{
    return exp(-0.5*(coord.x*coord.x+coord.y*coord.y)/sigma*sigma)/(2.0*3.14159265359*sigma*sigma);
}  

const float offset = 1.0 / 300.0;  

void main()
{
    
    //内核模糊和锐化图像
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );
    //锐化卷积内核
     float Sharpeningkernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
    

    //模糊卷积内核
    float Vaguekernel[9] = float[](
         1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
    );
    
    //边缘检测卷积内核
    float EdgeDetectionkernel[9] = float[](
        1, 1, 1,
        1, -8, 1,
        1, 1, 1
    );
    /*
    边缘检测原理
    增强边缘: 中心像素的权重为 -8，而周围的像素的权重为 1。如果一个像素的周围像素与其值相近（即没有边缘），
    加权和的结果会接近于中心像素的值。
    突出变化: 当中心像素与周围像素的值差异较大时（例如边缘），加权和会显著不同。
    这个差异会导致卷积结果为一个较大的正数或负数，表示边缘位置
    */

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(texture1, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
    {
        col += sampleTex[i] * EdgeDetectionkernel[i];
    }
      
    float brightness = (col.r + col.g + col.b) / 3.0; // 计算平均亮度
    if(brightness<0)
    {
       // FragColor = vec4(vec3(1.0,0.0,0.0), 1.0);
    }else{
        //FragColor = vec4(vec3(0.0,1.0,0.0), 1.0);
       
    }
     FragColor = vec4(col, 1.0);
    
   
    

    //倒置(返回这些纹理颜色的反转)
    // FragColor = vec4(vec3(1.0 - texture(texture1, TexCoords)), 1.0);


    
    //灰度
    //另一个有趣的效果是从场景中删除除白色、灰色和黑色之外的所有颜色;有效地对整个图像进行灰度化。
    //一种简单的方法是获取所有颜色分量并平均它们的结果：
    //FragColor = texture(texture1, TexCoords);
   // float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
   // FragColor = vec4(average, average, average, 1.0);

   //这已经产生了相当不错的效果，但人眼往往对绿色更敏感，而对蓝色最不敏感。
   //因此，为了获得最物理准确的结果，我们需要使用加权通道：
   // FragColor = texture(texture1, TexCoords);
   //float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
   // FragColor = vec4(average, average, average, 1.0);
   //您可能不会立即注意到差异,但对于更复杂的场景，这种加权灰度效果往往更加逼真


    /*
// 核的大小
    int kernelSize = 3; // 可以根据需要调整
    float weightSum = 0.0;
    vec3 result = vec3(0.0);
    
    // 遍历高斯核
    for (int x = -kernelSize; x<= kernelSize; x++) {
        for (int y = -kernelSize; y <= kernelSize; y++) {
            // 计算当前样本的纹理坐标
            vec2 offset = vec2(x, y)/2/kernelSize * blurSize;
            float weight = gaussian(offset);
            weightSum += weight;
            result += texture(texture1, TexCoords.st + offset).rgb * weight;
        }
    }

   FragColor = vec4(result/weightSum,1.0f);


*/
}  

