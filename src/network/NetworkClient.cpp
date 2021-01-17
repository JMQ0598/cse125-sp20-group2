#include <stdio.h>
#include <network/NetworkClient.h>

NetworkClient::NetworkClient(std::string host, int port)
{
    this->ConnectSocket = NetworkService::createClientSocket((char *) host.c_str(), port);
    /*if (this->ConnectSocket == INVALID_SOCKET) {
        exit(ERROR);
    }*/
}

NetworkClient::~NetworkClient()
{
    // shutdown(ConnectSocket, SD_SEND);
    closesocket(ConnectSocket);
    WSACleanup();
}

void NetworkClient::send(Game::ClientMessage clientMessage)
{
    // Send size and data
    size_t msgSize = clientMessage.ByteSizeLong();
    size_t bufSize = msgSize + sizeof(size_t);
    char buf[bufSize];
    memcpy(buf, &msgSize, sizeof(size_t));
    clientMessage.SerializeToArray(buf + sizeof(size_t), msgSize);

    NetworkService::sendMessage(this->ConnectSocket, buf, bufSize);
}

void NetworkClient::read() 
{
    // Get the number of bytes received from the server
    int receivedBytes = 1;

    while (receivedBytes > 0) {
        // Update receivedBytes
        receivedBytes = NetworkService::receiveMessage(this->ConnectSocket, network_data, DEFAULT_BUFLEN);

        // We got something
        if (receivedBytes > 0) {
            for (int i = 0; i < receivedBytes; i++) buffer.push_back(network_data[i]);
        }
    }

    // Create objects
    bool success = true;
    while(success) {

         // If the buffer size is less than a size of expected int we are reading
        if (buffer.size() <= sizeof(size_t)) {
            success = false;
            continue;
        }

        // This is the expected size of the ServerMessage
        size_t expectedSize;
        memcpy(&expectedSize, &buffer[0], sizeof(size_t));
        
        // Actual size of the buffer
        size_t actualSize = buffer.size() - sizeof(size_t);

        // Case 1: If it hasn't transferred enough, skip
        if (actualSize < expectedSize) {
            success = false;
            continue;
        }

        // Case 2: Read the msg, parse into object
        buffer.erase(buffer.begin(), buffer.begin() + sizeof(size_t));         

        Game::ServerMessage serverMessage;

        // Copy contents into array
        char objBuf[expectedSize];
        std::copy(buffer.begin(), buffer.begin() + expectedSize, objBuf);

        // Parse into a server message
        bool result = serverMessage.ParseFromArray(objBuf, expectedSize);

        // Erase the message from buffer
        buffer.erase(buffer.begin(), buffer.begin() + expectedSize);
        
        // Probably misaligned
        if (!result) {
            std::cerr << "Something bad happened!" << std::endl;
            exit(1);
        }

        // Store in messages of client
        messages.push_back(serverMessage);
    }
}