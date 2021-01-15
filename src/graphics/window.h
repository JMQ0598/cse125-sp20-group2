#pragma once

// Config
#include <util/Config.h>

// OpenGL function pointers
#include <glad/glad.h>		// Include this...
#include <GLFW/glfw3.h>		// ...before this.

// Std library packages
#include <iostream>
#include <unordered_map>

// ASSIMP
#include <assimp/config.h>

// Matrix operations and values
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

// Rendering classes
#include <graphics/render/Mesh.h>
#include <graphics/render/Shader.h>
#include <graphics/render/Camera.h>
#include <graphics/render/Model.h>

// Protobuf
#include <schema/Game.pb.h>

// Objects
#include <objects/GameObject.h>
#include <objects/Player.h>
#include <objects/Ingredient.h>
#include <objects/Plate.h>

#include <graphics/UIScreenFactory.h>

class Window
{
public:
    bool isClosed;
    bool showSettings;
    Window(int width, int height);

    std::unordered_map<int, GameObject*> objectsToRender;
    std::unordered_map<int, Ingredient*>* inventory;
    Ingredient* selectedIngredient = NULL;

    bool vodkaActive;

    void render();

    /**
     * @param object the game object to add to the map of objects
     * */
    void addObject(int id, GameObject* object);
    
    void removeObject(int index);

    void setTimer(int64_t timer);

    void setScore(int score);

    void setRound(int round);

    int getRound();

    Ingredient* getSelectedIngredient();

    void updateRound(Game::RoundInfo_RoundState);

    void addInventory(std::unordered_map<int, Ingredient*>* inventory);

    void toggleCursor();

    void addCookingEventMessage(std::string);

    void removeCookingEventMessage();

    void close();
    GLFWwindow* glfwViewport;
    bool gameOver = false;
    bool gameWin = false;
    Camera* camera;
    std::vector<std::string> instructionStrings;

    std::string recipeName;
    GLFWimage icons[1];

private:
    Shader* shader;
    Shader* UIshader;
    UIScreenFactory ui;
    // ImFont* font1;

    std::string cookingEventMsg = "";
    int64_t timer = 0;
    int round;
    int score = 0;
    void setupWindow();
    void cameraViewUpdate();
    unsigned int objNum;        // Increment when get response back from server
    int width;
    int height;
};