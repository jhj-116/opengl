#ifdef IGNORE_THIS_FILE
#else

#include <stb_image.h>

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
unsigned int loadTexture(const char* path);

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

    if (glewInit() != GLEW_OK)
    {
        std::cout << "glewInit is failed!" << std::endl;
        return 0;
    }
    
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    /*
    glStencilOp（GLenum sfail， GLenum dpfail， GLenum dppass） 包含三个选项，我们可以为每个选项指定要执行的操作：
    sfail：模板测试失败时要执行的操作。
    dpfail：如果模板测试通过，但深度测试失败，则要执行的操作。
    dppass：如果模板和深度测试都通过，则要执行的操作。
    然后，对于每个选项，您可以执行以下任一操作：
    
    */



    // build and compile shaders
    // -------------------------
    Shader shader("./res/shader/AdvancedOpengl/StencilTesting/stencil_testing_vs.shader", "./res/shader/AdvancedOpengl/StencilTesting/stencil_testing_fs.shader");
    Shader shaderSingleColor("./res/shader/AdvancedOpengl/StencilTesting/stencil_testing_vs.shader", "./res/shader/AdvancedOpengl/StencilTesting/stencil_single_color_fs.shader");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cubeVertices[] = {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    float planeVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };
    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    // load textures
    // -------------
    const std::string cubeTexturePath = "./res/texture/container2.png";

    const std::string floorTexturePath = "./res/texture/ai.jpg";
    unsigned int cubeTexture = loadTexture(cubeTexturePath.c_str());
    unsigned int floorTexture = loadTexture(floorTexturePath.c_str());

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("texture1", 0);

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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // don't forget to clear the stencil buffer!
        
        // set uniforms
        shaderSingleColor.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shaderSingleColor.setMat4("view", view);
        shaderSingleColor.setMat4("projection", projection);
       // shaderSingleColor.setMat4("model", glm::mat4(1.0f));

        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        // draw floor as normal, but don't write the floor to the stencil buffer, we only care about the containers. We set its mask to 0x00 to not write to the stencil buffer.
        glStencilMask(0x00);
        // floor
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        shader.setMat4("model", glm::mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // 1st. render pass, draw objects as normal, writing to the stencil buffer
        // --------------------------------------------------------------------
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        /*
        glStencilFunc（GLenum func， GLint ref， GLuint mask） 有三个参数：
        func：设置模板测试函数，该函数确定片段是通过还是被丢弃。此测试函数应用于存储的模具值和 glStencilFunc 的值。
        可能的选项包括： 、 、 、 。这些的语义含义类似于深度缓冲区的函数。refGL_NEVERGL_LESSGL_LEQUALGL_GREATERGL_GEQUALGL_EQUALGL_NOTEQUALGL_ALWAYS
        ref：指定模板测试的参考值。将模板缓冲区的内容与此值进行比较。
        mask：指定一个掩码，在测试比较它们之前，该掩码同时包含引用值和存储的模板值。最初设置为所有

        但 glStencilFunc 仅描述 OpenGL 是否应根据模板缓冲区的内容传递或丢弃片段，而不描述我们如何实际更新缓冲区。
        这就是 glStencilOp 的用武之地
        */

        glStencilMask(0xFF);
     
        /*
        函数 glStencilMask 允许我们设置一个位掩码，该位掩码与即将写入缓冲区的模板值一起执行。
        它用于设置模板测试中模板缓冲区的写入掩码。它的主要作用是控制哪些位可以被写入模板缓冲区，影响后续的模板测试和操作
        // 允许写入所有位
        glStencilMask(0xFF);
        只允许写入模板缓冲区的低四位
        glStencilMask(0x0F);
        // 进行其他渲染操作，不会修改模板缓冲区的高四位
        */

        // cubes
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
        // Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
        // the objects' size differences, making it look like borders.
        // -----------------------------------------------------------------------------------------------------------------------------
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);//若是模版缓冲区值不等于1的话，表示通过，等于1表示不通过，
        //丢弃该模版缓冲区值为1的片段，并将通过的所有模版缓冲区值更新为1
        /*
        如果将0xFF掩码改为 0x00，即使用 glStencilFunc(GL_NOTEQUAL, 1, 0x00);，则表示在比较中没有有效的位。
        这意味着模板比较不会检查任何位。因此，任何模板值都会被视为通过测试，因为没有位被用来进行比较。
        */

        glStencilMask(0x00);//上述glStencilFunc(GL_NOTEQUAL, 1, 0xFF);将通过的所有模版缓冲区值更新为1，但是
        // glStencilMask(0x00);这里掩码设置表示可以只读取模板值而不进行修改，不希望改变模板缓冲区的内容。

        //这里展示Cube轮廓其实是绘制了4个Cube，其中两个Cube是带纹理，两个Cube是不带纹理，只有vec4(0.04, 0.28, 0.26, 1.0)颜色
        //通过设置模版缓冲区函数(glStencilFunc)和glStencilOp(对模版缓冲区值进行操作)，进行改变设置片段的每个像素的模版缓冲区值
        //来绘制高光效果(带纹理的Cube比颜色Cube小,就是好展现展示轮廓效果)


       /* glStencilFunc(GL_EQUAL, 0, 0xFF);
        glStencilMask(0x00); glStencilMask(0xFF);*/
        glDisable(GL_DEPTH_TEST);
        shaderSingleColor.use();
        float scale = 1.1f;//这有个缩放说明后面绘制的vec4(0.04, 0.28, 0.26, 1.0)颜色的Cube比有纹理的Cube大
        // cubes
        glBindVertexArray(cubeVAO);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));//model = glm::translate(model, glm::vec3(-2.0f, 0.0f, -2.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        shaderSingleColor.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));// model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        shaderSingleColor.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //模板缓冲区的默认值通常是 0，但可以通过清除操作来设置为其他值
        //glClearStencil(1); // 设置清除模板缓冲区时的值为 1
        //glClear(GL_STENCIL_BUFFER_BIT); // 清除模板缓冲区

        glBindVertexArray(0);
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);

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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
1.模板测试
片段着色器处理完片段后，将执行所谓的模板测试，就像深度测试一样，该测试可以选择丢弃片段。
之后，剩余的片段将传递到深度测试，OpenGL 可能会丢弃更多的片段。
模具测试基于另一个称为模具缓冲区的缓冲区的内容，我们被允许在渲染期间更新该缓冲区以实现有趣的效果。

2.使用模板缓冲区时，您可以随心所欲地疯狂，但一般过程通常如下：
启用写入模板缓冲区。
渲染对象，更新模板缓冲区的内容。
禁用对模板缓冲区的写入。
渲染（其他）对象，这次根据模板缓冲区的内容丢弃某些片段

glEnable(GL_STENCIL_TEST);启用模版缓冲区
请注意，您还需要在每次迭代时清除模板缓冲区，就像颜色和深度缓冲区一样：
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


3.在 OpenGL 中，模板测试（Stencil Testing）是一种强大的技术，用于控制像素的渲染。
它通过模板缓冲区提供了一种额外的测试机制，允许开发者实现复杂的视觉效果。以下是模板测试的主要用途和功能：
主要用途
遮罩效果：
模板测试可以用于创建遮罩效果，例如只在特定形状或区域内渲染对象。这对于实现透明效果、阴影、光照等非常有用。
多重渲染：
可以在同一帧中多次渲染到同一片段。通过使用模板缓冲区，可以控制哪些片段会被渲染，从而实现复杂的效果，比如在一个对象上绘制多个效果（如高光、阴影）。
场景分割：
模板测试可以用于场景的分割和分层渲染。不同的物体可以被分配到不同的模板值，从而在后续渲染时使用模板测试进行选择性渲染。
创建复杂形状：
可以使用模板测试来创建复杂的形状或切割物体。例如，可以在一个物体上切割出另一个物体的形状，或者进行布尔运算（如相交、并、差集）。
动态效果：
在游戏或交互式应用中，可以动态地改变模板缓冲区的内容，以实现实时效果。例如，可以根据用户输入或动画状态改变哪些区域被渲染。


https://learnopengl.com/Advanced-OpenGL/Stencil-testing


*/