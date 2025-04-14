#ifdef IGNORE_THIS_FILE
#else
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader/shader_m.h>
#include <package/Camera.hpp>
#include <package/ModelLoading/Model/model.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
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

    if (glewInit() != GLEW_OK)
        std::cout << "glewInit is failed" << std::endl;

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);



    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("./res/shader/AdvancedOpengl/Depth/depth_model_loading_vs.shader", "./res/shader/AdvancedOpengl/Depth/depth_model_loading_fs.shader");

    // load models
    // -----------
    const std::string ourModelPath = "./res/model/backpack/backpack.obj";
    Model ourModel(ourModelPath.c_str());


    // draw in wireframe
    //启用线框模式
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
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
#endif // !

/*
深度缓冲区是一个缓冲区，就像颜色缓冲区（存储所有片段颜色：视觉输出）一样，存储每个片段的信息，
并且具有与颜色缓冲区相同的宽度和高度。深度缓冲区由窗口系统自动创建，并将其深度值存储为 或 bit floats。
在大多数系统中，您会看到精度为 bits 的深度缓冲区

启用深度测试后，OpenGL 会根据深度缓冲区的内容测试片段的深度值。OpenGL 执行深度测试，如果此测试通过，则会渲染片段，并使用新的深度值更新深度缓冲区。如果深度测试失败，则会丢弃该片段。

深度测试是在片段着色器运行之后（以及我们将在下一章中介绍的模板测试之后）在屏幕空间中完成的。
屏幕空间坐标与 OpenGL 的 glViewport 函数定义的视区直接相关，并且可以通过片段着色器中 GLSL 的内置变量进行访问。
的 x 和 y 分量表示片段的屏幕空间坐标（其中 （0,0） 是左下角）。
该变量还包含一个 z 分量，其中包含片段的深度值。此 z 值是与深度缓冲区内容进行比较的值


启用后，如果片段通过深度测试，OpenGL 会自动将其 z 值存储在深度缓冲区中，如果片段未通过深度测试，则丢弃片段。
如果您启用了深度测试，则还应在每帧之前使用 ;否则，您将卡在上一帧的 depth 值上


glDepthMask(GL_FALSE);//在某些情况下，您希望对所有片段执行深度测试并相应地丢弃它们，但不更新深度缓冲区。
基本上，您（暂时）使用的是只读深度缓冲区。
OpenGL 允许我们通过将其深度掩码设置为来禁用对深度缓冲区的写入：GL_FALSE,仅在启用深度测试时有效

深度测试功能
OpenGL 允许我们修改它用于深度测试的比较运算符。这允许我们控制 OpenGL 何时应传递或丢弃片段，以及何时更新深度缓冲区。
我们可以通过调用 glDepthFunc 来设置比较运算符（或深度函数）


默认情况下，使用 depth 函数丢弃 depth 值大于或等于当前深度缓冲区值的所有片段。默认深度函数(glDepthFunc)的设置运算符w为GL_LESS

+
深度值精度公式：F(depth)=(z-near)/(far-near)
（线性）方程将 z 值转换为介于 和 之间的深度值,这里的near和far一定是我们视图矩阵时选择的
投影矩阵以设置可见视锥体的 near 和 far 值

非线性的方程(深度值公式)：F(depth)=(1/z−1/near)/(1/far−1/near)
如果您不知道这个方程式的确切情况，请不要担心。要记住的重要一点是，深度缓冲区中的值在剪辑空间中不是线性的（在应用投影矩阵之前，
它们在视图空间中是线性的）。
深度缓冲区中的值并不意味着像素的 z 值位于视锥体的一半;顶点的 z 值实际上非常接近近平面
https://learnopengl.com/Advanced-OpenGL/Depth-testing

Z深度缓存线性与非线性推导公式地址:https://blog.csdn.net/sinat_29158831/article/details/123993642
*/