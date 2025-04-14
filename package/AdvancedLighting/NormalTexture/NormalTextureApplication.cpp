#ifdef IGNORE_THIS_FILE
#else
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <package/Camera.hpp>
#include <shader/shader_m.h>
#include <package/ModelLoading/Model/model.h>

#include <iostream>

#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
void renderQuad();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (glewInit()!=GLEW_OK)
    {
        std::cout << "Failed to initialize glewInit" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader shader("./res/shader/AdvancedLighting/NormalTexture/normal_mapping_vs.shader", "./res/shader/AdvancedLighting/NormalTexture/normal_mapping_fs.shader");

    // load textures
    // -------------

    const std::string diffuseMapPath = "./res/texture/brickwall.jpg";
    const std::string normalMapPath = "./res/texture/brickwall_normal.jpg";
    unsigned int diffuseMap = loadTexture(diffuseMapPath.c_str());
    unsigned int normalMap = loadTexture(normalMapPath.c_str());

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("diffuseMap", 0);
    shader.setInt("normalMap", 1);

    // lighting info
    // -------------
    glm::vec3 lightPos(0.5f, 1.0f, 0.3f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // configure view/projection matrices
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // render normal-mapped quad
        glm::mat4 model = glm::mat4(1.0f);

        //这里让Cube模型进行 glm::rotate绕平面x,z旋转
        //单独绕那一轴(x,y,z)，那一轴的坐标不变。这里绕一个平面旋转(x,z)应该x,y,z会变
 
        model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
        shader.setMat4("model", model);
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightPos", lightPos);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap);
        renderQuad();

        // render light source (simply re-renders a smaller plane at the light's position for debugging/visualization)
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        shader.setMat4("model", model);
        renderQuad();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// renders a 1x1 quad in NDC with manually calculated tangent vectors
// ------------------------------------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        // positions
        glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3(1.0f, -1.0f, 0.0f);
        glm::vec3 pos4(1.0f, 1.0f, 0.0f);
        // texture coordinates
        glm::vec2 uv1(0.0f, 1.0f);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(1.0f, 0.0f);
        glm::vec2 uv4(1.0f, 1.0f);
        // normal vector
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        // calculate tangent/bitangent vectors of both triangles
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
        // triangle 1
        // ----------
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        // triangle 2
        // ----------
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);


        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);


        float quadVertices[] = {
            // positions            // normal         // texcoords  // tangent                          // bitangent
            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
        };
        // configure plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
#endif
/*
是的，在一个有水泥墙纹理的模型上贴上法线贴图通常会显著提高视觉效果。以下是几个原因，说明为什么法线贴图能够改善水泥墙的表现：
1. 增加细节感
水泥墙通常有许多微小的凹凸和不规则性，这些细节在法线贴图中可以被模拟出来。通过使用法线贴图,
模型表面的细微细节（如裂缝、颗粒和不均匀的纹理）可以被增强，而不需要增加多边形数量。
2. 改善光照效果
法线贴图可以改变模型表面的法线方向，从而影响光照的计算。水泥墙的光照通常会因表面不平整而产生变化，
法线贴图能够更真实地模拟这些光照效果，增加墙面的立体感和真实感。
3. 减少多边形数量
如果你想让水泥墙看起来更复杂，直接增加多边形数量可能会导致性能下降。使用法线贴图可以在不增加多边形的情况下创造出复杂的视觉效果，
从而提高性能，尤其在实时渲染场景（如游戏）中尤为重要。
4. 增强材质质感
法线贴图可以与其他贴图（如漫反射贴图、镜面反射贴图）结合使用，提供更丰富的材质表现。
例如，水泥墙的法线贴图可以使其看起来更粗糙和真实，增强整体的视觉质量。

2.为什么低边模型用了法线贴图，可以让低边模型变高模?
低多边形模型使用法线贴图可以让其在视觉上表现得像高多边形模型，这主要归功于以下几个机制：
1. 细节模拟
法线贴图能够在低多边形模型上模拟复杂的表面细节（如凹凸和纹理），而不需要实际增加多边形的数量。
通过对表面法线的修改，法线贴图可以创造出微小的光照变化，使得表面看起来更加丰富和复杂。
2. 光照效果的改善
法线贴图改变了表面的法线方向，从而影响光照和阴影的计算。即使在低多边形模型上，法线贴图也能使得光照效果更加细腻，
增强了立体感和深度感。这使得低多边形模型在不同光照条件下展现出更高的细节。
3. 性能优化
在实时渲染（如游戏）中，直接使用高多边形模型会对性能造成较大压力。低多边形模型配合法线贴图，可以在保持视觉效果的同时，
显著降低渲染负担。这种技术使得开发者能够创建复杂的场景而不会牺牲性能。
4. 与其他贴图结合使用
法线贴图通常与其他类型的贴图（如漫反射贴图、镜面反射贴图等）结合使用，
通过综合多种贴图的效果，进一步提升模型的视觉表现。这种组合可以使低多边形模型在材质和外观上更加真实。
5. 艺术风格的适应
法线贴图可以根据艺术风格的需求进行调整，使得低多边形模型在不同风格下也能保持良好的视觉效果。
无论是现实主义风格还是卡通风格，法线贴图都能有效增强模型的细节表现。
小结
通过使用法线贴图，低多边形模型能够在视觉上接近高多边形模型的效果。
这使得开发者可以在保持性能的同时，创建出更具细节和真实感的3D场景，是现代3D渲染技术中的重要手段。


三:
在计算机图形学中如何使用纹理来存储法线信息，以实现法线映射（normal mapping）。以下是逐步解释：
每个片段的法线：为了使法线映射有效，需要为每个片段（即每个像素）获取一个法线向量。
使用2D纹理：与漫反射和镜面反射贴图类似，可以使用一个2D纹理来存储每个片段的法线数据。这样，渲染时可以从纹理中取样，以获得该片段的法线向量。
法线向量与纹理：法线向量是几何实体，而纹理通常用于存储颜色信息。将法线向量存储在纹理中可能不太直观，
但实际上可以通过将法线向量的x、y和z分量分别映射到纹理的红色、绿色和蓝色分量来实现。
法线范围映射：法线向量的值范围通常在-1到1之间，因此在存储之前，需要将这些值映射到[0, 1]的范围。这意味着：
将法线的x分量从[-1, 1]映射到[0, 1]，可以使用公式：mapped_x = (original_x + 1) / 2。
同理，对于y和z分量也进行相同的处理。
通过这种方式，法线向量可以有效地存储在纹理中，并在渲染时被提取和使用。


四：
为什么需要在切线空间中计算？
使用切线空间进行光照计算的原因包括：
法线贴图的效果：法线贴图的法线向量通常是在切线空间中定义的，
因此在计算光照时需要将所有相关向量（光源、视点等）转换到切线空间，以确保它们与法线贴图中的法线向量正确匹配。
局部细节：切线空间允许更好地处理表面上的局部细节，使得光照效果更加真实。
通过这种方式，法线贴图可以有效地影响光照和表面细节的表现，提升渲染的真实感。


五：法线贴图不使用切线空间造成的问题？
这讨论了使用法线贴图时遇到的一个问题，以及如何通过切线空间（tangent space）解决这个问题。以下是逐步解释：
1. 法线贴图的方向问题
法线贴图通常包含的法线向量大多指向正z方向。这在平面表面法线也指向正z方向时是有效的。
但是，如果将相同的法线贴图应用于一个平放在地面上的平面（此时表面法线指向正y方向,法线贴图通常包含的法线向量指向正z方向,与表面法线不一致），就会出现问题。
2. 错误的光照效果
在这种情况下，虽然平面的法线应该指向正y方向，但采样到的法线仍然大致指向正z方向。
因此，光照计算会错误地认为表面的法线与之前指向正z方向的情况相同，导致光照效果不正确。
这意味着光照模型无法正确反映平面的真实表面方向，从而产生错误的视觉效果。
3. 解决方案
多张法线贴图：一种解决方案是为每个可能的表面方向定义一张法线贴图。
例如，如果是立方体，就需要六张法线贴图。这在简单情况中是可行的，
但对于复杂的网格（可能有数百种表面方向）来说，这种方法不可行。
切线空间：另一种解决方案是将所有光照计算在一个不同的坐标空间中进行，即切线空间。
在这个空间中，法线贴图中的法线始终指向正z方向。所有其他光照向量（如光源方向、视点方向,模型顶点坐标等）则相对于这个正z方向进行变换。
这样，无论表面如何旋转，都能够始终使用相同的法线贴图，从而简化了处理和保证了正确的光照效果。

总结
这段文字的核心是说明在使用法线贴图时，表面的方向会影响光照效果。
通过使用切线空间，可以解决这一问题，使得无论表面如何旋转，都能正确地使用相同的法线贴图，从而保证光照计算的准确性。
*/