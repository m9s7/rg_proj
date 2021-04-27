#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/model.h>

#include <iostream>
#include <model_manager.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
glm::mat4 static_camera();
void setViewAndProjectionMatrixForAllShaders(vector<Shader*> &shaders);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

glm::mat4 drawStand(unsigned int VAO, glm::mat4 &model, Shader shader, int indices_count);

unsigned selectedStand = 0;
glm::vec3 *light_position;

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

    /* DRAWING OBJECT WITH EBO */
    float ground_vertices[] = {
            0.5f, 0.1f, 0.5f, 0.0f, 1.0f, 0.0f,     //A0
            0.5f, 0.1f, -0.5f, 0.0f, 1.0f, 0.0f,    //B1
            -0.5f, 0.1f, 0.5f, 0.0f, 1.0f, 0.0f,    //C2

            0.5f, 0.1f, -0.5f, 0.0f, 1.0f, 0.0f,    //B1
            -0.5f, 0.1f, 0.5f, 0.0f, 1.0f, 0.0f,     //C2
            -0.5f, 0.1f, -0.5f, 0.0f, 1.0f, 0.0f,   //D3

            0.5f, 0.1f, 0.5f, 1.0f, 0.0f, 0.0f,      //A0
            0.5f, 0.1f, -0.5f, 1.0f, 0.0f, 0.0f,     //B1
            0.5f, -0.1f, 0.5f, 1.0f, 0.0f, 0.0f,     //E4

            0.5f, 0.1f, -0.5f, 1.0f, 0.0f, 0.0f,     //B1
            0.5f, -0.1f, 0.5f, 1.0f, 0.0f, 0.0f,     //E4
            0.5f, -0.1f, -0.5f, 1.0f, 0.0f, 0.0f,    //F5

            0.5f, 0.1f, -0.5f, 0.0f, 0.0f, 1.0f,     //B1
            -0.5f, 0.1f, -0.5f, 0.0f, 0.0f, 1.0f,    //D3
            0.5f, -0.1f, -0.5f, 0.0f, 0.0f, 1.0f,    //F5

            -0.5f, 0.1f, -0.5f, 0.0f, 0.0f, 1.0f,    //D3
            0.5f, -0.1f, -0.5f, 0.0f, 0.0f, 1.0f,    //F5
            -0.5f, -0.1f, -0.5f, 0.0f, 0.0f, 1.0f,   //H7

            -0.5f, 0.1f, 0.5f, -1.0f, 0.0f, 0.0f,     //C2
            -0.5f, 0.1f, -0.5f, -1.0f, 0.0f, 0.0f,    //D3
            -0.5f, -0.1f, 0.5f, -1.0f, 0.0f, 0.0f,    //G6

            -0.5f, 0.1f, -0.5f, -1.0f, 0.0f, 0.0f,    //D3
            -0.5f, -0.1f, 0.5f,  -1.0f, 0.0f, 0.0f,   //G6
            -0.5f, -0.1f, -0.5f, -1.0f, 0.0f, 0.0f,   //H7

            0.5f, 0.1f, 0.5f, 0.0f, 0.0f, -1.0f,      //A0
            -0.5f, 0.1f, 0.5f, 0.0f, 0.0f, -1.0f,     //C2
            0.5f, -0.1f, 0.5f, 0.0f, 0.0f, -1.0f,     //E4

            -0.5f, 0.1f, 0.5f, 0.0f, 0.0f, -1.0f,     //C2
            0.5f, -0.1f, 0.5f, 0.0f, 0.0f, -1.0f,     //E4
            -0.5f, -0.1f, 0.5f, 0.0f, 0.0f, -1.0f,    //G6

            0.5f, -0.1f, 0.5f, 0.0f, -1.0f, 0.0f,     //E4
            0.5f, -0.1f, -0.5f, 0.0f, -1.0f, 0.0f,    //F5
            -0.5f, -0.1f, 0.5f, 0.0f, -1.0f, 0.0f,    //G6

            0.5f, -0.1f, -0.5f, 0.0f, -1.0f, 0.0f,    //F5
            -0.5f, -0.1f, 0.5f, 0.0f, -1.0f, 0.0f,    //G6
            -0.5f, -0.1f, -0.5f, 0.0f, -1.0f, 0.0f,   //H7
    };

    int indices_count = 36;
    unsigned int VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ground_vertices), ground_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    /* Stand set up */
    Shader groundShader("resources/shaders/ground_shader.vs", "resources/shaders/ground_shader.fs");
    Shader selectedStandShader("resources/shaders/selected_shader.vs", "resources/shaders/selected_stand.fs");

    glm::vec3 stand_position_0 = glm::vec3(-3.9f, -0.5f, 0.0f);
    glm::vec3 stand_position_1 = glm::vec3(1.3f, 0.0f, 0.0f);
    glm::vec3 stand_position_2 = glm::vec3(1.3f, 0.0f, 0.0f);
    glm::vec3 stand_position_3 = glm::vec3(1.3f, 0.0f, 0.0f);

    // Stand model 0 - initial - kakashi
    glm::mat4 standModel_0 = glm::mat4(1.0f);
    standModel_0 = glm::translate(standModel_0, stand_position_0);
//    standModel_0 = glm::rotate(standModel_0, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    standModel_0 = glm::scale(standModel_0, glm::vec3(2.0f, 1.0f, 2.0f));
    // Stand model 1 - sasuke
    glm::mat4 standModel_1 = glm::translate(standModel_0, stand_position_1);
    // Stand model 1 - naruto
    glm::mat4 standModel_2 = glm::translate(standModel_1, stand_position_2);
    // Stand model 1 - sakura
    glm::mat4 standModel_3 = glm::translate(standModel_2, stand_position_3);

    vector<glm::mat4> stand_models = {standModel_0, standModel_1, standModel_2, standModel_3};

    /*Model set up*/
    Shader modelShader("resources/shaders/model_shader.vs", "resources/shaders/model_shader.fs");

    ModelManager mm = ModelManager();

    /* Shaders */
    vector<Shader*> shaders = {&groundShader, &modelShader, &selectedStandShader};


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

        /* render the loaded models */ //- can all be moved to 1 function

        mm.setSelectedModel(static_cast<Character>(selectedStand));

        // Set light position
        switch (selectedStand) {
            case 0: light_position = &stand_position_0; break;
            case 1: light_position = &stand_position_1; break;
            case 2: light_position = &stand_position_2; break;
            case 3: light_position = &stand_position_3; break;
            default: cout << "greska" << endl;
        }

        // Draw characters
//        modelShader.setVec3("lightPos", *light_position);
        mm.drawCharacter(KAKASHI, modelShader);
        mm.drawCharacter(SASUKE, modelShader);
        mm.drawCharacter(NARUTO, modelShader);
        mm.drawCharacter(SAKURA, modelShader);

        /* Draw stands */
        for(unsigned i = 0; i < stand_models.size(); i++) {
            if (i == selectedStand)
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
    shader.setVec3("lightPos", *light_position);
    shader.setMat4("model", model);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, indices_count);
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
