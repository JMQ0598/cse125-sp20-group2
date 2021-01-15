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
#include <SFML/Audio.hpp>
#include <util/Config.h>

class ClientGame {
    public:
        // This is the client id.
        // NOTE: This does NOT correlate directly with any objects!!!
        unsigned int clientId;

        // This is the object id.
        // NOTE: This is the id that correlates to this client's player object. 
        unsigned int objectId;
        unsigned int round = 0;
        sf::Music music;
        sf::Sound soundEffect;
        sf::SoundBuffer soundBuffer;

        ClientGame(std::string IP, int port);
        ~ClientGame();
        void setupAndRun();
        void runGame();
        
        // Used to register single key presses
        void keyBindsHandler(GLFWwindow* window, int key, int scancode, int action, int mods);

    private:
        NetworkClient client;
        Window* window;
        GameObject* winner;
        void receiveUpdates();
        void updateGameState();
        void processInput();
};