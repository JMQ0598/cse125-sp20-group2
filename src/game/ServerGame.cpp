#include <game/ServerGame.h>
#include <util/PrintUtil.h>
#include <thread>
#include <chrono>

#define TICK 1000

ServerGame::ServerGame(int port) : server(port)
{
    // std::function<void(int)> test = std::bind(&ServerGame::acall, this)
    std::function<void(int)> test = std::bind(&ServerGame::acceptCallback, this, std::placeholders::_1);
    this->server.setAcceptCallback(test);

    run();
}

ServerGame::~ServerGame()
{

}

void ServerGame::run()
{
    while (true) {
        auto start = std::chrono::high_resolution_clock::now();

        this->process();

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> dur = end - start;

        std::chrono::duration<double> sleepTime = std::chrono::milliseconds(TICK) - dur;

        if (sleepTime.count() >= 0) {
            std::cout << "Time to process: " << dur.count() << " Sleeping for: " << sleepTime.count() << std::endl;
            std::this_thread::sleep_for(sleepTime);
        } else {
            std::cout << "Server is behind by: " << dur.count() << " no panic " << std::endl;
        }
    }
}

void ServerGame::process() 
{
    // TODO: We should have some sort of GameLogic handling class.
    // GameState should not be handling the modification of the data. It should hold the data.
    // A GameLogic handler will take in these messages and calculate what needs to be done.
    auto map = server.readAllMessages();
    
    // Go through all clients
    // This is just an example. This isn't necessarily the correct logic.
    for (auto iter = map.begin(); iter != map.end(); iter++) {
        auto clientId = iter->first;
        auto msgs = iter->second;
        for (auto msg: msgs) {
            PrintUtil::print(msg);

            std::cout << "iteration" << std::endl;
            switch (msg.event_case()) {
                case Game::ClientMessage::EventCase::kDirection:
                {
                    auto & vector = this->gameState.playerLocations[clientId];
                    switch (msg.direction()) {
                        case Game::Direction::UP:
                        vector->set_x(vector->x() + 1);
                        break;
                        case Game::Direction::DOWN:
                        vector->set_x(vector->x() - 1);
                        break;
                        case Game::Direction::LEFT:
                        vector->set_z(vector->z() - 1);
                        break;
                        case Game::Direction::RIGHT:
                        vector->set_z(vector->z() + 1);
                        break;
                    }
                }
                default:
                {

                }
            }
        }
        
        Game::ServerMessage serverMsg;

        Game::Vector3 *test = new Game::Vector3();
        auto vector = this->gameState.playerLocations[clientId];
        test->set_x(vector->x());
        test->set_y(vector->y());
        test->set_z(vector->z());
        serverMsg.set_allocated_vector(test);
        this->server.send(clientId, serverMsg);
        
        // CRASHES
        // serverMsg.set_allocated_vector(this->gameState.playerLocations[clientId]);
        // std::cout << "DO WE EVER REACH HERE" << std::endl;
        // this->server.send(clientId, serverMsg);
    }
}

void ServerGame::acceptCallback(int clientId) 
{
    this->gameState.addUser(clientId);
}