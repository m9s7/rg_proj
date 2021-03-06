//
// Created by ubuntu on 4/26/21.
//

#ifndef PROJECT_BASE_MODEL_MANAGER_H
#define PROJECT_BASE_MODEL_MANAGER_H

#include <learnopengl/model.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

enum Character{KAKASHI = 0, SASUKE = 1, NARUTO = 2, SAKURA = 3};

class MyModel {
public:
    Character character;
    Model *model{};
    glm::mat4 modelMatrix = glm::mat4(1.0f);;
    glm::mat4 initialModelMatrix;

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
        std::memcpy(glm::value_ptr(initialModelMatrix), glm::value_ptr(modelMatrix), sizeof(modelMatrix));
    }

    void resetModelMatrix(){
        std::memcpy(glm::value_ptr(modelMatrix), glm::value_ptr(initialModelMatrix), sizeof(initialModelMatrix));
    }

private:
    void makeKakashi(){
        model = new Model(FileSystem::getPath("resources/objects/kakashi/D0401273.obj"));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-3.9f, -0.4f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
    }
    void makeSasuke(){
        model = new Model(FileSystem::getPath("resources/objects/sasuke/sasuke.obj"));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.3f, -0.4f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.105f));
    }
    void makeNaruto(){
        model = new Model(FileSystem::getPath("resources/objects/naruto/D0401253.obj"));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(1.3f, -0.4f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
    }
    void makeSakura(){
        model = new Model(FileSystem::getPath("resources/objects/sakura/sakura.obj"));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(3.9f, -0.4f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.016f));
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
    Shader &shader;

    explicit ModelManager(Shader &shader1) : shader(shader1) {}

    void drawCharacter(Character modelType){
        shader.setMat4("model", models[modelType]->modelMatrix);
        getModel(modelType)->Draw(shader);
    }

    void drawCharachters(float rotateVal){
        models[selectedModel]->modelMatrix = glm::rotate(models[selectedModel]->modelMatrix,
                                                         glm::radians(rotateVal),
                                                         glm::vec3(0.0f, 1.0f, 0.0f));
        shader.use();
        drawCharacter(KAKASHI);
        drawCharacter(SASUKE);
        drawCharacter(NARUTO);
        drawCharacter(SAKURA);
    }

    Model* getModel(Character modelType){
        return models[modelType]->model;
    }

    void setSelectedModel(Character type){
        if(this->selectedModel != type){
            models[selectedModel]->resetModelMatrix();
            this->selectedModel = type;
        }
    }
};

#endif //PROJECT_BASE_MODEL_MANAGER_H
