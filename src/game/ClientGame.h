#pragma once

#include <network/NetworkClient.h>
#include <iostream>
#include <util/PrintUtil.h>
#include <graphics/window.h>
#include <chrono>
#include <thread>
#include <util/MessageBuilder.h>
#include <util/Config.h>
#include <util/MapBuilder.h>

class ClientGame {
    public:
        // This is the client id.
        // NOTE: This does NOT correlate directly with any objects!!!
        unsigned int clientId;

        // This is the object id.
        // NOTE: This is the id that correlates to this client's player object. 
        unsigned int objectId;
        unsigned int round = 0;

        ClientGame(std::string IP, int port);
        ~ClientGame();
        void runGame();

        // Used to register single key presses
        void keyBindsHandler(GLFWwindow* window, int key, int scancode, int action, int mods);

    private:
        NetworkClient client;
        Window window;
        void receiveUpdates();
        void updateGameState();
        void processInput();
};