#pragma once

#include <network/NetworkService.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <thread>
#include <functional>
#include <queue>
#include <schema/Game.pb.h>
#include <util/Config.h>
#include <util/MessageBuilder.h>

class ServerNetwork
{
public:

    /**
     * Initializes the ServerNetwork. This will setup listening to the respective port.
     * These sockets are non-blocking.
     * @param port the port to bind to
     * */
    ServerNetwork(int port);

    ~ServerNetwork();
    
    /**
     * Sends a server message to a client. 
     * @param clientId The client to send a message to
     * @param msg The message to send
     * */
    void send(unsigned int clientId, Game::ServerMessage message);

    /**
     * Sends a server message to all clients.
     * 
     * @param msg The message to send
     * */
    void sendToAll(Game::ServerMessage message);

    /**
     * Sends a server message to a specific socket.
     * @param socket The socket to send a message to
     * @param msg The message to send
     * */
    void sendToSocket(SOCKET socket, Game::ServerMessage message);

    /**
     * Reads all messages queued in the socket.
     * 
     * @returns a map of clientId to client messages. 
     * */
    std::unordered_map<unsigned int, std::vector<Game::ClientMessage>> readAllMessages();

    /**
     * Sets a callback to ServerGame to set up a player
     * once they have connected
     * */
    void setOnClientConnect(std::function<void(int)> func);

    /**
     * Sets a callback to ServerGame to remove a player
     * once they have disconnected
     * */
    void setOnClientDisconnect(std::function<void(int)> func);

private:
    /**
     * The socket the server binds to.
     * */
    SOCKET listenSocket;

    /**
     * The thread the server uses to accept new clients.
     * */ 
    std::thread threadAccept;

    /**
     * This holds temporary data while reading from the socket.
     * */
    char network_data[DEFAULT_BUFLEN];

    /**
     * Used to keep track of the client id's avaliable for use
     * */
    std::queue<unsigned int> clientIdQueue;

    /**
     * This is the list of users who are connected.
     * This is a mapping of clientId to their respective socket.
     * */
    std::map<unsigned int, SOCKET> sessions;

    /**
     * This holds temporary data while reading from the socket. 
     * After being copied to buffer from the respective socket, we use a temporary buffer to queue up data. 
     * This handles cases where we receive LESS than the expected size.
     * */
    std::map<unsigned int, std::vector<char>> buffers;

    unsigned int getNextId();

    /**
     * Creates a new socket for any connecting clients, and
     * adds them to "sessions" under its client id.
     * */
    void startAccepting();

    /**
     * This function is called when a new client is accepted. 
     * Calls the function with the incoming clientId.
     * IMPORTANT: This is called on another thread.
     * */
    std::function<void(int)> onClientConnect;

    /**
     * This function is called when a client has disconnected.
     * */
    std::function<void(int)> onClientDisconnect;

    /**
     * Makes changes to remove client both from servernetwork and servergame
     * Closes the socket, removes the clientId from sessions,
     * tells servergame to remove the player, and puts the clientId up for others
     * */
    void removeClient(unsigned int clientId);

    /**
     * Makes changes to add client both to servernetwork and servergame
     * adds the clientId to sessions and tells servergame to add the player
     * */
    void addClient(unsigned int clientId, SOCKET clientSocket);
};