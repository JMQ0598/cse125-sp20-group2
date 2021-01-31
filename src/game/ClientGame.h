#pragma once

#include <iostream>
#include <util/PrintUtil.h>
#include <graphics/window.h>
#include <objects/Wall.h>
#include <objects/Cookware.h>
#include <objects/Plate.h>
#include <util/IngredientFactory.h>
#include <objects/Player.h>
#include <objects/Table.h>
#include <objects/GameObject.h>
#include <chrono>
#include <thread>
#include <util/Config.h>
#include <stack>

class ClientGame {
    public:

        unsigned int round = 0;

        ClientGame(int i);
        ~ClientGame();
        void runGame();

        // Used to register single key presses
        void keyBindsHandler(GLFWwindow* window, int key, int scancode, int action, int mods);

    private:
        Window window;
        void receiveUpdates();
        void updateGameState();
        void processInput();

        ///NOTE: Dev tool for tracking where the next object will be placed (cursor)
        /// Should be the only object with index 0.
        GameObject* cursor = NULL;
        int cursorId = 0;

        ///NOTE: Dev tool for tracking objects
        std::stack<GameObject*> mapObjects;
        GameObject* lastDeleted = NULL;
        int playerIndex = -2;

        ///NOTE: Dev tool for map building inputs.
        void mapbuildingInput(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods);

        ///NOTE: Dev tool for exporting map files.
        void exportMapTxt();
};