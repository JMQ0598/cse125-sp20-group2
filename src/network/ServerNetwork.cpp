#include <iostream>
#include <stdlib.h>
#include <ws2tcpip.h>
#include <string>
#include <network/ServerNetwork.h>

ServerNetwork::ServerNetwork(int port)
{
    this->listenSocket = NetworkService::createServerSocket(port);
    this->threadAccept = std::thread (&ServerNetwork::startAccepting, this);

    // Get max players from config, add ids to queue
    int playerCount = Config::getInt("Max_Players");
    for (int i = 0; i < playerCount; i++) {
        clientIdQueue.push(i);
    }
}

ServerNetwork::~ServerNetwork()
{
    std::map<unsigned int, SOCKET>::iterator mapIter;
    for (mapIter = sessions.begin(); mapIter != sessions.end(); mapIter++)
    {
        SOCKET currSocket = mapIter->second;
        // let peer know that server is shutting down
        shutdown(currSocket, SD_SEND);
        closesocket(currSocket);
    }
    closesocket(listenSocket);
    WSACleanup();
}

unsigned int ServerNetwork::getNextId()
{
    unsigned int clientId = clientIdQueue.front();
    clientIdQueue.pop();
    return clientId;
}

void ServerNetwork::startAccepting()
{
    std::cout << "Accepting Clients\n";
    while (1) {
        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
        if (WSAGetLastError() == WSAEWOULDBLOCK) {
            continue;
        }

        if (clientSocket == INVALID_SOCKET) {
            std::cout << "Accept failed with error:" << WSAGetLastError() << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return;
        }

        // Empty, force disconnect client
        if (clientIdQueue.empty())
        {
            std::cout << "no more player slots available, closing the connection" << std::endl;
            Game::ServerMessage* msg = MessageBuilder::toServerDisconnectMsg(Config::get("Max_Player_Disconnect_Msg"));
            sendToSocket(clientSocket, *msg);
            delete msg;
            closesocket(clientSocket);
        }
        // Slot available, continue
        else
        {
            std::cout << "slot available, continuing" << std::endl;
            unsigned int clientId = getNextId();
            std::cout << "Accepted Client #" << clientId << std::endl;
            this->addClient(clientId, clientSocket);
        }
    }
}

std::unordered_map<unsigned int, std::vector<Game::ClientMessage>> ServerNetwork::readAllMessages()
{
    std::unordered_map<unsigned int, std::vector<Game::ClientMessage>> map;
    std::map<unsigned int, SOCKET>::iterator iter;

    if (sessions.empty()) {
        return map;
    }
    
    for (iter = sessions.begin(); iter != sessions.end(); iter++) {
        unsigned int clientId = iter->first;
        SOCKET socket = iter->second;
        int bytesReceived;

        // Read message into a queue to parse
        do {
            bytesReceived = NetworkService::receiveMessage(socket, network_data, DEFAULT_BUFLEN);

            // Client disconnect case
            if (bytesReceived == 0)
            {
                if (this->onClientDisconnect) {
                    this->removeClient(clientId);
                }
            }

            // Received data case
            if (bytesReceived > 0) {
                for (int i = 0; i < bytesReceived; i++) buffers[clientId].push_back(network_data[i]);
            }
        }
        while (bytesReceived > 0);

        // First, need to figure out the vector<char> size
        std::vector<char> & buffer = buffers[clientId];
        bool success = true;
        while (success) {
            // If the buffer size is less than a size of expected int we are reading
            if (buffer.size() <= sizeof(size_t)) {
                success = false;
                continue;
            }

            // This is the expected size of the ClientMessage
            size_t expectedSize;
            memcpy(&expectedSize, &buffer[0], sizeof(size_t));

            // This is the actual size we have from the buffer
            size_t actualSize = buffer.size() - sizeof(size_t);

            // Case 1: If it hasn't transferred enough, skip
            if (actualSize < expectedSize) {
                success = false;
                continue;
            }
            
            // Case 2: Read the msg, parse into object
            buffer.erase(buffer.begin(), buffer.begin() + sizeof(size_t));            
            Game::ClientMessage clientMessage;

            // Copy contents into array
            char objBuf[expectedSize];
            std::copy(buffer.begin(), buffer.begin() + expectedSize, objBuf);

            // Parse into client message
            bool result = clientMessage.ParseFromArray(objBuf, expectedSize);
            
            // Erase the message from buffer
            buffer.erase(buffer.begin(), buffer.begin() + expectedSize);

            // We received corrupt data that should not have happened
            // Assume client is friendly and data is correctly formatted
            if (!result) {
                std::cerr << "Something bad happened!" << std::endl;
                exit(1);
            }

            map[clientId].push_back(clientMessage);
        }
    }
    return map;
}

void ServerNetwork::send(unsigned int clientId, Game::ServerMessage message)
{
    size_t msgSize = message.ByteSizeLong();
    size_t bufSize = msgSize + sizeof(size_t);
    char buf[bufSize];
    memcpy(buf, &msgSize, sizeof(size_t));
    message.SerializeToArray(buf + sizeof(size_t), msgSize);

    NetworkService::sendMessage(this->sessions[clientId], buf, bufSize);
}

void ServerNetwork::sendToSocket(SOCKET socket, Game::ServerMessage message)
{
    size_t msgSize = message.ByteSizeLong();
    size_t bufSize = msgSize + sizeof(size_t);
    char buf[bufSize];
    memcpy(buf, &msgSize, sizeof(size_t));
    message.SerializeToArray(buf + sizeof(size_t), msgSize);

    NetworkService::sendMessage(socket, buf, bufSize);
}

void ServerNetwork::sendToAll(Game::ServerMessage message) 
{
    for (auto pair: this->sessions) {
        unsigned int clientId = pair.first;
        this->send(clientId, message);
    }
}

void ServerNetwork::setOnClientConnect(std::function<void(int)> func) 
{
    this->onClientConnect = func;
}

void ServerNetwork::setOnClientDisconnect(std::function<void(int)> func)
{
    this->onClientDisconnect = func;
}


void ServerNetwork::removeClient(unsigned int clientId)
{
    std::cout << "servernetwork -> remove client method is called" << std::endl;
    
    SOCKET clientSocket = sessions[clientId];
    closesocket(clientSocket);

    sessions.erase(clientId);

    if(this->onClientDisconnect) {
        this->onClientDisconnect(clientId);
    }

    clientIdQueue.push(clientId);
}

void ServerNetwork::addClient(unsigned int clientId, SOCKET clientSocket)
{
    sessions.insert(std::pair<unsigned int, SOCKET>(clientId, clientSocket));

    if (this->onClientConnect) {
        this->onClientConnect(clientId);
    }
}