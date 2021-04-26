//
// Created by ubuntu on 4/26/21.
//

#ifndef PROJECT_BASE_MODEL_MANAGER_H
#define PROJECT_BASE_MODEL_MANAGER_H

#include <learnopengl/model.h>

enum Character{KAKASHI = 0, SASUKE = 1, NARUTO = 2, SAKURA = 3};

struct MyModel {
    Character character;
    Model *model;
    glm::mat4 modelMatrix;
//    glm::mat4 standMatrix;

    explicit MyModel(Character character)
    {
        this->character = character;
        switch (character) {
            case KAKASHI: makeKakashi(); break;
            case SASUKE: makeSasuke(); break;
            case NARUTO: makeNaruto(); break;
            case SAKURA: makeSakura(); break;
            default: std::cout << "Invalid character selected" << std::endl;
        }
    }
    void makeKakashi(){
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-3.9f, -0.4f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));

        model = new Model(FileSystem::getPath("resources/objects/kakashi/D0401273.obj"));
    }
    void makeSasuke(){
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.3f, -0.4f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.105f));

        model = new Model(FileSystem::getPath("resources/objects/sasuke/sasuke.obj"));
    }
    void makeNaruto(){
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(1.3f, -0.4f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));

        model = new Model(FileSystem::getPath("resources/objects/naruto/D0401253.obj"));
    }
    void makeSakura(){
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(3.6f, 0.4f, 7.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(-80.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f));

        model = new Model(FileSystem::getPath("resources/objects/sakura/sakura.obj"));
    }
};

class ModelManager{

public:
    Character selectedModel = KAKASHI;
    vector<MyModel*> models = {
            new MyModel(KAKASHI),
            new MyModel(SASUKE),
            new MyModel(NARUTO),
            new MyModel(SAKURA)
    };
    ModelManager()= default;

    void drawCharacter(Character modelType, Shader &shader){
        shader.use();
//        if(modelType == selectedModel){
//            auto modelMat = getModelMatrix(modelType);
//            modelMat = glm::rotate(modelMat, glm::radians(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
//        }
        shader.setMat4("model", getModelMatrix(modelType));
        getModel(modelType)->Draw(shader);
    }

    glm::mat4& getModelMatrix(Character modelType){
        return models[modelType]->modelMatrix;
    }
    Model* getModel(Character modelType){
        return models[modelType]->model;
    }


};

#endif //PROJECT_BASE_MODEL_MANAGER_H
