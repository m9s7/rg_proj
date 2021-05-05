#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/model.h>
#include <learnopengl/camera.h>

#include <iostream>
#include <model_manager.h>

float* initStandVertices(unsigned &size);
float* initCubemapVertices(unsigned &size);
unsigned int loadTexture(const char *path);
unsigned int loadCubemapTexture();

void setModelShaderUniforms(Shader &shader);
void setStandShaderUniforms(Shader &shader);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);

void setViewAndProjectionMatrixForAllShaders(vector<Shader*> &shaders);

void initPodiumModelMatrices(vector<glm::mat4> &standModels, vector<glm::vec3> &standPosition);
glm::mat4 drawStand(unsigned int VAO, glm::mat4 &model, Shader shader, int indices_count);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 2.0f, 10.0f));
double lastX = SCR_WIDTH / 2.0f;
double lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned selectedStand = 0;
glm::vec3 *light_position;

int main()
{
    /* INIT GLFW, MAKE A WINDOW AND SET CALLBACK FUNCTIONS */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Team 7 Wax Figurines", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    /* ENABLE FACE CULLING */
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // govori koji face se odseca
    glFrontFace(GL_CCW); // govori sta je front face

    /* MAKE STAND EBO */
    unsigned sizeof_standVertices;
    float* standVertices = initStandVertices(sizeof_standVertices);

    int indices_count = 36;
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof_standVertices, standVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Shader groundShader("resources/shaders/ground_shader.vs", "resources/shaders/ground_shader.fs");
    Shader selectedStandShader("resources/shaders/selected_shader.vs", "resources/shaders/selected_stand.fs");

    unsigned int diffuseMap = loadTexture(FileSystem::getPath("resources/textures/rocks/Rock_Mosaic_DIFF.png").c_str());
    unsigned int specularMap = loadTexture(FileSystem::getPath("resources/textures/rocks/Rock_Mosaic_SPEC.png").c_str());

    groundShader.use();
    groundShader.setInt("material.diffuse", 0);
    groundShader.setInt("material.specular", 1);

    /* MAKE CUBEBOX EBO */

    unsigned sizeof_cubemapVertices;
    float* cubemapVertices = initCubemapVertices(sizeof_cubemapVertices);

    unsigned int cubemapVAO, cubemapVBO;
    glGenVertexArrays(1, &cubemapVAO);
    glGenBuffers(1, &cubemapVBO);
    glBindVertexArray(cubemapVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubemapVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof_cubemapVertices, cubemapVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) nullptr);
    glEnableVertexAttribArray(0);

    Shader cubemapShader("resources/shaders/cubemap.vs", "resources/shaders/cubemap.fs");
    cubemapShader.setInt("cubemap", 0);

    unsigned int cubemapTexture = loadCubemapTexture();

    vector<glm::mat4> standModels;
    vector<glm::vec3> standPosition;
    initPodiumModelMatrices(standModels, standPosition);

    /* MAKING MODELS */
    Shader modelShader("resources/shaders/multiple_lights.vs", "resources/shaders/multiple_lights.fs");
    ModelManager mm = ModelManager(modelShader);

    // draw in wireframe
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /* RENDER LOOP */
    vector<Shader*> shaders = {&groundShader, &modelShader, &selectedStandShader, &cubemapShader};
    while (!glfwWindowShouldClose(window))
    {
        /* initial set up */
        auto currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        /* process input */
        processInput(window);
        mm.setSelectedModel(static_cast<Character>(selectedStand));
        light_position = &standPosition[selectedStand];

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        setViewAndProjectionMatrixForAllShaders(shaders);

        /* draw models */
        setModelShaderUniforms(modelShader);
        mm.drawCharachters(deltaTime*100);

        /* draw stands */

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        setStandShaderUniforms(groundShader);

        /* Draw stands */
        for(unsigned i = 0; i < standModels.size(); i++) {
            if (i == selectedStand)
                drawStand(VAO, standModels[i], selectedStandShader, indices_count);
            else
                drawStand(VAO, standModels[i], groundShader, indices_count);
        }

        /* cube mapping */

        cubemapShader.use();
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        glBindVertexArray(cubemapVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    delete standVertices;
    delete cubemapVertices;
    return 0;
}

void setViewAndProjectionMatrixForAllShaders(vector<Shader*> &shaders){
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                            (float)SCR_WIDTH / (float)SCR_HEIGHT,
                                            0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();

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
        standModels.push_back(glm::translate(glm::mat4(1.0f), i));
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        camera.ProcessMouseMovement(0.0, 10.0);
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        camera.ProcessMouseMovement(0.0, -10.0);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessMouseMovement(50.0, 0.0);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessMouseMovement(-50.0, 0.0);

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
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    (void)xoffset;
    (void)window;
    camera.ProcessMouseScroll(yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
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

unsigned int loadCubemapTexture() {

    vector<std::string> faces = {
            FileSystem::getPath("resources/textures/my_cubemap/right.png"),
            FileSystem::getPath("resources/textures/my_cubemap/left.png"),
            FileSystem::getPath("resources/textures/my_cubemap/top.png"),
            FileSystem::getPath("resources/textures/my_cubemap/down.png"),
            FileSystem::getPath("resources/textures/my_cubemap/front.png"),
            FileSystem::getPath("resources/textures/my_cubemap/back.png"),
    };

    unsigned int textureID;
    glGenBuffers(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int widht, height, nrChannels;
    unsigned char* data;

    for(int i = 0; i < (int)faces.size(); i++){
        data = stbi_load(faces[i].c_str(), &widht, &height, &nrChannels, 0);
        if(data){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, widht, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }else{
            cout << "failed to load cubemap texture" << endl;
            return -1;
        }
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

float* initStandVertices(unsigned &size){
    unsigned numOfVert = 36;
    unsigned numOfCol = 8;

    size = numOfVert * numOfCol * sizeof(float);
    auto* vertices = new float[numOfVert * numOfCol]{
        // vert(3), norm(3), tex(2)

        // top
        1.0f, 0.1f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,      //A0
        1.0f, 0.1f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,     //B1
        -1.0f, 0.1f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,     //C2

        1.0f, 0.1f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,     //B1
        -1.0f, 0.1f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,    //D3
        -1.0f, 0.1f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,     //C2

        // right
        1.0f, 0.1f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.2f,     //B1
        1.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.2f,      //A0
        1.0f, -0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     //E4

        1.0f, 0.1f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.2f,     //B1
        1.0f, -0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     //E4
        1.0f, -0.1f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,    //F5

        // back
        -1.0f, 0.1f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.2f,    //D3
        1.0f, 0.1f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.2f,     //B1
        1.0f, -0.1f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,    //F5

        -1.0f, 0.1f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.2f,    //D3
        1.0f, -0.1f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,    //F5
        -1.0f, -0.1f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   //H7

        // left
        -1.0f, 0.1f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.2f,    //C2
        -1.0f, 0.1f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.2f,   //D3
        -1.0f, -0.1f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   //G6

        -1.0f, -0.1f, 1.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  //G6
        -1.0f, 0.1f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.2f,   //D3
        -1.0f, -0.1f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  //H7

        // front
        1.0f, 0.1f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.2f,     //A0
        -1.0f, 0.1f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.2f,    //C2
        1.0f, -0.1f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,    //E4

        1.0f, -0.1f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,    //E4
        -1.0f, 0.1f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.2f,    //C2
        -1.0f, -0.1f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,   //G6

        // bottom
        1.0f, -0.1f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   //F5
        1.0f, -0.1f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,    //E4
        -1.0f, -0.1f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,   //G6

        1.0f, -0.1f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   //F5
        -1.0f, -0.1f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,   //G6
        -1.0f, -0.1f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  //H7
    };

    return vertices;
}

float* initCubemapVertices(unsigned &size){
    unsigned numOfVert = 36;
    unsigned numOfCol = 3;

    size = numOfVert * numOfCol * sizeof(float);
    auto* vertices = new float[numOfVert * numOfCol]{
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
    };

    return vertices;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    (void)window;
    // Meni ovde ozbiljne nebuloze vraca sistem za x i y poziciju misa
    // ja nmg nikako da namestim da ovo proradi
//    cout << xpos << " " << ypos << endl;
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

//    double xoffset = xpos - lastX;
//    double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;


//    camera.ProcessMouseMovement(xoffset, yoffset);
}

void setModelShaderUniforms(Shader &shader){
    shader.use();

    shader.setVec3("pointLight.position", *light_position);
    shader.setVec3("cameraPos", camera.Position);

    shader.setVec3("dirLight.direction", 0.0f, -1.0f, -1.0f);
    shader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
    shader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
    shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    shader.setVec3("pointLight.ambient", 0.2f, 0.2f, 0.2f);
    shader.setVec3("pointLight.diffuse", 1.0f, 1.0f, 1.0f);
    shader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);

    shader.setFloat("pointLight.constant", 0.5f);
    shader.setFloat("pointLight.linear", 0.09f);
    shader.setFloat("pointLight.quadratic", 0.032f);
}

void setStandShaderUniforms(Shader &shader){
    shader.use();

    shader.setVec3("pointLight.position", *light_position);
    shader.setVec3("cameraPos", camera.Position);

    shader.setVec3("dirLight.direction", 0.0f, -1.0f, 1.0f);
    shader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
    shader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
    shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    shader.setVec3("pointLight.ambient", 0.2f, 0.2f, 0.2f);
    shader.setVec3("pointLight.diffuse", 1.0f, 1.0f, 1.0f);
    shader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);

    shader.setFloat("pointLight.constant", 0.5f);
    shader.setFloat("pointLight.linear", 0.09f);
    shader.setFloat("pointLight.quadratic", 0.032f);
}