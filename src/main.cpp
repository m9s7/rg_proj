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
void initPodiumModelMatrices(vector<glm::mat4> &standModels, vector<glm::vec3> &standPosition);
glm::mat4 initPodiumModelMatrix(glm::vec3 stand_position);
unsigned int loadTexture(const char *path);

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

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // govori koji face se odseca
    glFrontFace(GL_CCW); // govori sta je front face
    // CW = BACKFACE
    // CCW = FRONTFACE

    /* DRAWING OBJECT WITH EBO */
    float ground_vertices[] = {
            // vert(3), norm(3), tex(2)
            // ovo je top
            0.5f, 0.05f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,    //A0
            0.5f, 0.05f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,    //B1
            -0.5f, 0.05f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,   //C2

            0.5f, 0.05f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,    //B1
            -0.5f, 0.05f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,   //D3
            -0.5f, 0.05f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,     //C2

            // ovo je desno
            0.5f, 0.05f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.2f,    //B1
            0.5f, 0.05f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.2f,     //A0
            0.5f, -0.05f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,    //E4

            0.5f, 0.05f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.2f,    //B1
            0.5f, -0.05f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,    //E4
            0.5f, -0.05f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   //F5

            // Ovo je back
            -0.5f, 0.05f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.2f,   //D3
            0.5f, 0.05f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.2f,    //B1
            0.5f, -0.05f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,   //F5

            -0.5f, 0.05f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.2f,   //D3
            0.5f, -0.05f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,   //F5
            -0.5f, -0.05f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  //H7

            // ovo je levo
            -0.5f, 0.05f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.2f,    //C2
            -0.5f, 0.05f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.2f,   //D3
            -0.5f, -0.05f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   //G6

            -0.5f, -0.05f, 0.5f,  -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  //G6
            -0.5f, 0.05f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.2f,   //D3
            -0.5f, -0.05f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  //H7

            // ovo je front
            0.5f, 0.05f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.2f,     //A0
            -0.5f, 0.05f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.2f,    //C2
            0.5f, -0.05f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,    //E4

            0.5f, -0.05f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,    //E4
            -0.5f, 0.05f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.2f,    //C2
            -0.5f, -0.05f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,   //G6

            // ovo je bottom
            0.5f, -0.05f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   //F5
            0.5f, -0.05f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,     //E4
            -0.5f, -0.05f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,   //G6

            0.5f, -0.05f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,    //F5
            -0.5f, -0.05f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,   //G6
            -0.5f, -0.05f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  //H7
    };

    int indices_count = 36;
    unsigned int VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ground_vertices), ground_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    /* Stand set up */
    Shader groundShader("resources/shaders/ground_shader.vs", "resources/shaders/ground_shader.fs");
    Shader selectedStandShader("resources/shaders/selected_shader.vs", "resources/shaders/selected_stand.fs");

    vector<glm::mat4> standModels;
    vector<glm::vec3> standPosition;
    initPodiumModelMatrices(standModels, standPosition);

    /*Model set up*/
    Shader modelShader("resources/shaders/multiple_lights.vs", "resources/shaders/multiple_lights.fs");

    ModelManager mm = ModelManager();

    /* Shaders */
    vector<Shader*> shaders = {&groundShader, &modelShader, &selectedStandShader};

    // Loading texture
    unsigned int diffuseMap = loadTexture(FileSystem::getPath("resources/textures/rocks/Rock_Mosaic_DIFF.png").c_str());
    unsigned int specularMap = loadTexture(FileSystem::getPath("resources/textures/rocks/Rock_Mosaic_SPEC.png").c_str());

    groundShader.use();
    groundShader.setInt("material.diffuse", 0);
    groundShader.setInt("material.specular", 1);

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
        light_position = &standPosition[selectedStand];

        modelShader.use();

        modelShader.setVec3("pointLight.position", light_position->x, light_position->y, light_position->z);

        modelShader.setVec3("dirLight.direction", 0.0f, -1.0f, -1.0f);
        modelShader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
        modelShader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
        modelShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        modelShader.setVec3("pointLight.ambient", 0.2f, 0.2f, 0.2f);
        modelShader.setVec3("pointLight.diffuse", 1.0f, 1.0f, 1.0f);
        modelShader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);

        modelShader.setFloat("pointLight.constant", 0.5f);
        modelShader.setFloat("pointLight.linear", 0.09f);
        modelShader.setFloat("pointLight.quadratic", 0.032f);

        // Draw characters
        mm.drawCharacter(KAKASHI, modelShader);
        mm.drawCharacter(SASUKE, modelShader);
        mm.drawCharacter(NARUTO, modelShader);
        mm.drawCharacter(SAKURA, modelShader);

        // Postavimo teksturu i proprties za groundShader

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        groundShader.use();

        groundShader.setVec3("pointLight.position", light_position->x, light_position->y, light_position->z);

        groundShader.setVec3("dirLight.direction", 0.0f, -1.0f, 1.0f);
        groundShader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
        groundShader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
        groundShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        groundShader.setVec3("pointLight.ambient", 0.2f, 0.2f, 0.2f);
        groundShader.setVec3("pointLight.diffuse", 1.0f, 1.0f, 1.0f);
        groundShader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);

        groundShader.setFloat("pointLight.constant", 0.5f);
        groundShader.setFloat("pointLight.linear", 0.09f);
        groundShader.setFloat("pointLight.quadratic", 0.032f);

        /* Draw stands */
        for(unsigned i = 0; i < standModels.size(); i++) {
            if (i == selectedStand)
                drawStand(VAO, standModels[i], selectedStandShader, indices_count);
            else
                drawStand(VAO, standModels[i], groundShader, indices_count);
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
    shader.setMat4("model", model);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, indices_count);
    return model;
}

void initPodiumModelMatrices(vector<glm::mat4> &standModels, vector<glm::vec3> &standPosition){
    standPosition.emplace_back(-3.9f, -0.5f, 0.0f);
    standPosition.emplace_back(-1.3f, -0.5f, 0.0f);
    standPosition.emplace_back(1.3f, -0.5f, 0.0f);
    standPosition.emplace_back(3.9f, -0.5f, 0.0f);

    for(auto & i : standPosition)
        standModels.push_back(initPodiumModelMatrix(i));
}

glm::mat4 initPodiumModelMatrix(glm::vec3 stand_position){
    glm::mat4 standModel = glm::mat4(1.0f);
    standModel = glm::translate(standModel, stand_position);
    standModel = glm::scale(standModel, glm::vec3(2.0f));
    return standModel;
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
    if(key == GLFW_KEY_E && action == GLFW_PRESS){
        light_position->y = 100.0f;
        cout << light_position->x << " ";
        cout << light_position->y << " ";
        cout << light_position->z << " ";
        cout <<endl;
    }

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

unsigned int loadTexture(const char *path){
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;

    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if(!data){
        cout << "Texture failed to load" << endl;
        stbi_image_free(data);
        return textureID;
    }

    GLint format;
    if(nrComponents == 2)
        format = GL_RED;
    else if(nrComponents == 3)
        format = GL_RGB;
    else if(nrComponents == 4)
        format = GL_RGBA;
    else{
        cout << "Greska u ucitavanju texture (format)" << endl;
        return textureID;
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_LINEAR_MIPMAP_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return textureID;
}
