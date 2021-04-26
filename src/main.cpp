#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
glm::mat4 static_camera();
void setViewAndProjectionMatrixForAllShaders(vector<Shader*> &shaders);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

glm::mat4 drawStand(unsigned int VAO, glm::mat4 &model, Shader shader, int indices_count);

glm::mat4 initKakashiModel();
glm::mat4 initSasukeModel();
glm::mat4 initNarutoModel();
glm::mat4 initSakuraModel();

unsigned selectedStand = 0;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//// camera
//float lastX = SCR_WIDTH / 2.0f;
//float lastY = SCR_HEIGHT / 2.0f;
//bool firstMouse = true;

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "RG projekat", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

//    // tell GLFW to capture our mouse - ovo ti skloni kursor sa ekrana i to retardirano
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
//    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader modelShader("resources/shaders/model_shader.vs", "resources/shaders/model_shader.fs");

    // load models
    // -----------
    Model kakashi(FileSystem::getPath("resources/objects/kakashi/D0401273.obj"));
    Model sasuke(FileSystem::getPath("resources/objects/sasuke/sasuke.obj"));
    Model naruto(FileSystem::getPath("resources/objects/naruto/D0401253.obj"));
    Model sakura(FileSystem::getPath("resources/objects/sakura/sakura.obj"));

    Shader groundShader("resources/shaders/ground_shader.vs", "resources/shaders/ground_shader.fs");
    Shader selectedStandShader("resources/shaders/selected_shader.vs", "resources/shaders/selected_stand.fs");

    vector<Shader*> shaders = {&groundShader, &modelShader, &selectedStandShader};


    /* DRAWING OBJECT WITH EBO */
    float ground_vertices[] = {
            // above
            0.5f, 0.1f, 0.5f,       //A0
            0.5f, 0.1f, -0.5f,      //B1
            -0.5f, 0.1f, 0.5f,      //C2
            -0.5f, 0.1f, -0.5f,     //D3
            // below
            0.5f, -0.1f, 0.5f,      //E4
            0.5f, -0.1f, -0.5f,     //F5
            -0.5f, -0.1f, 0.5f,     //G6
            -0.5f, -0.1f, -0.5f,    //H7
    };
    unsigned ground_indices[] = {
        0, 1, 2,
        1, 2, 3,

        4, 5, 6,
        5, 6, 7,

        0, 1, 4,
        1, 4, 5,

        1, 3, 5,
        3, 5, 7,

        2, 3, 6,
        3, 6, 7,

        0, 2, 4,
        2, 4, 6
    };
    int indices_count = sizeof(ground_indices)/sizeof(ground_indices[0]);
    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ground_vertices), ground_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ground_indices), ground_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    /* Stand set up */
    // Stand model 0 - initial - kakashi
    glm::mat4 standModel_0 = glm::mat4(1.0f);
    standModel_0 = glm::translate(standModel_0, glm::vec3(-3.9f, -0.5f, 0.0f));
    standModel_0 = glm::rotate(standModel_0, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    standModel_0 = glm::scale(standModel_0, glm::vec3(2.0f, 1.0f, 2.0f));
    // Stand model 1 - sasuke
    glm::mat4 standModel_1 = glm::translate(standModel_0, glm::vec3(1.3f, 0.0f, 0.0f));
    // Stand model 1 - naruto
    glm::mat4 standModel_2 = glm::translate(standModel_1, glm::vec3(1.3f, 0.0f, 0.0f));
    // Stand model 1 - sakura
    glm::mat4 standModel_3 = glm::translate(standModel_2, glm::vec3(1.3f, 0.0f, 0.0f));

    vector<glm::mat4> stand_models = {standModel_0, standModel_1, standModel_2, standModel_3};

    // draw in wireframe
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        /* initial set up */
        auto currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        setViewAndProjectionMatrixForAllShaders(shaders);

        /* render the loaded models */
        modelShader.use();

            // kakashi
        glm::mat4 kakashiModel = initKakashiModel();
        modelShader.setMat4("model", kakashiModel);
        kakashi.Draw(modelShader);

            // sasuke
        glm::mat4 sasukeModel = initSasukeModel();
        modelShader.setMat4("model", sasukeModel);
        sasuke.Draw(modelShader);

            // naruto
        glm::mat4 narutoModel = initNarutoModel();
        modelShader.setMat4("model", narutoModel);
        naruto.Draw(modelShader);

            // sakura
        glm::mat4 sakuraModel = initSakuraModel();
        modelShader.setMat4("model", sakuraModel);
        sakura.Draw(modelShader);

        /* Draw stands */
        for(unsigned i = 0; i < stand_models.size(); i++){
            if(i == selectedStand)
                drawStand(VAO, stand_models[i], selectedStandShader, indices_count);
            else
                drawStand(VAO, stand_models[i], groundShader, indices_count);
        }

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

glm::mat4 initKakashiModel(){
    glm::mat4 kakashiModel = glm::mat4(1.0f);
    kakashiModel = glm::translate(kakashiModel, glm::vec3(-3.9f, -0.4f, 0.0f));
    kakashiModel = glm::scale(kakashiModel, glm::vec3(0.1f));
    return kakashiModel;
}
glm::mat4 initSasukeModel(){
    glm::mat4 sasukeModel = glm::mat4(1.0f);
    sasukeModel = glm::translate(sasukeModel, glm::vec3(-1.3f, -0.4f, 0.0f));
    sasukeModel = glm::scale(sasukeModel, glm::vec3(0.105f));
    return sasukeModel;
}
glm::mat4 initNarutoModel(){
    glm::mat4 narutoModel = glm::mat4(1.0f);
    narutoModel = glm::translate(narutoModel, glm::vec3(1.3f, -0.4f, 0.0f));
    narutoModel = glm::scale(narutoModel, glm::vec3(0.1f));
    return narutoModel;
}
glm::mat4 initSakuraModel(){
    glm::mat4 sakuraModel = glm::mat4(1.0f);
    sakuraModel = glm::translate(sakuraModel, glm::vec3(3.6f, 0.4f, 7.0f));
    sakuraModel = glm::rotate(sakuraModel, glm::radians(-80.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    sakuraModel = glm::scale(sakuraModel, glm::vec3(2.0f));
    return sakuraModel;
}


void setViewAndProjectionMatrixForAllShaders(vector<Shader*> &shaders){
    glm::mat4 view = static_camera();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    for(Shader* shader : shaders){
        shader->use();
        shader->setMat4("projection", projection);
        shader->setMat4("view", view);
    }
}


glm::mat4 drawStand(unsigned int VAO, glm::mat4 &model, Shader shader, int indices_count){
    shader.use();
    shader.setMat4("model", model);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, nullptr);
    return model;
}

void processInput(GLFWwindow *window){
    (void)window;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    (void)scancode;
    (void)mods;
    if(action == GLFW_RELEASE || action == GLFW_REPEAT)
        return;
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS && selectedStand < 3)
        selectedStand++;
    if(key == GLFW_KEY_LEFT && action == GLFW_PRESS && selectedStand > 0)
        selectedStand--;

    std::cout << selectedStand << std::endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
}

glm::mat4 static_camera(){
    glm::vec3 Position = glm::vec3(0.0f, 2.0f, 10.0f);
    glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Right = glm::normalize(glm::cross(Front, WorldUp));
    glm::vec3 Up = glm::normalize(glm::cross(Right, Front));
    return glm::lookAt(Position, Position + Front, Up);
}
