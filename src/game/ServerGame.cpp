#include <game/ServerGame.h>
#include <processors/GameProcessor.h>
#include <stdlib.h>
#include <algorithm>

ServerGame::ServerGame(int port) : server(port)
{
    // Initialize the gamestate 
    this->onRoundChange();
    
    // Set up function to initialize players
    std::function<void(int)> notifyClients = std::bind(&ServerGame::onClientConnect, this, std::placeholders::_1);
    this->server.setOnClientConnect(notifyClients);

    // Set up a function to remove players
    std::function<void(int)> removeClients = std::bind(&ServerGame::onClientDisconnect, this, std::placeholders::_1);
    this->server.setOnClientDisconnect(removeClients);
    run();
}

ServerGame::~ServerGame()
{

}

int ServerGame::getTick() 
{
    return TICK;
}

void ServerGame::run()
{
    while (true) {
        auto start = std::chrono::high_resolution_clock::now();

        auto msgMap = server.readAllMessages();
        
        this->preprocess(msgMap);

        this->process(msgMap);

        this->update();

        this->sendPendingMessages();

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> dur = end - start;

        std::chrono::duration<double> sleepTime = std::chrono::milliseconds(TICK) - dur;

        if (sleepTime.count() >= 0) {
            // std::cout << "Time to process: " << dur.count() << " Sleeping for: " << sleepTime.count() << std::endl;
            std::this_thread::sleep_for(sleepTime);
        } else {
            std::cout << "Server is behind by: " << dur.count() << " no panic " << std::endl;
        }
    }
}


void ServerGame::preprocess(std::unordered_map<unsigned int, std::vector<Game::ClientMessage>> & clientMsgs)
{
    for (auto & it : clientMsgs)
    {
        std::vector<Game::ClientMessage> copy = it.second;
        std::vector<Game::Direction> directionCache;

        std::vector<Game::ClientMessage> & msgs = it.second;
        msgs.clear();

        for (Game::ClientMessage msg : copy) {
            if (!msg.has_direction()) {
                msgs.push_back(msg);
                continue;
            }

            if (!std::count(directionCache.begin(), directionCache.end(), msg.direction())) {
                msgs.push_back(msg);
                directionCache.push_back(msg.direction());
            }
        }

        // OLD CODE
        // std::vector<Game::ClientMessage> & msgList = it.second;
        // auto uniquePortion = std::unique(msgList.begin(), msgList.end(), google::protobuf::util::MessageDifferencer::Equals);
        // msgList.erase(uniquePortion, msgList.end());
    }
}

void ServerGame::process(std::unordered_map<unsigned int, std::vector<Game::ClientMessage>> & msgMap) 
{
    /// TODO: Different ways to advance round based on the different rounds.
    if (this->gameState.gameOver()) {
        this->gameState.advanceRound();
        this->onRoundChange();
        return;
    }

    // Go through all clients
    for (auto iter = msgMap.begin(); iter != msgMap.end(); iter++) {
        
        auto clientId = iter->first;
        auto msgs = iter->second;

        for (auto msg : msgs) {

            switch (this->gameState.getRound()) {
                case Game::RoundInfo::LOBBY:
                {
                    GameProcessor::processLobby(clientId, msg, this);
                    GameProcessor::process(clientId, msg, this);
                    break;
                }
                case Game::RoundInfo::DUNGEON_WAITING:
                {
                    break;
                }
                case Game::RoundInfo::DUNGEON:
                {
                    GameProcessor::process(clientId, msg, this);
                    break;
                }
                case Game::RoundInfo::KITCHEN_WAITING:
                {
                    break;
                }
                case Game::RoundInfo::KITCHEN:
                {
                    GameProcessor::process(clientId, msg, this);
                    break;
                }
                case Game::RoundInfo::END:
                {
                    GameProcessor::process(clientId, msg, this);
                    break;
                }
                default:
                {
                    std::cout << "Round state is not handled" << std::endl;
                }
            }
        }
    }
}

/// TODO: USE FOR TIMER LOGIC AND EVENTS THAT DEPEND ON TIME
void ServerGame::update()
{
    // Time updates
    if (this->gameState.timeHasUpdated())
    {
        int roundTime = this->gameState.getRoundTime();
        Game::ServerMessage* serverMsg = MessageBuilder::toTimeMessage(roundTime);
        this->messages.push_back(serverMsg);
    }

    /// TODO: yes
    // Check some data structure (timer value), create any pending messages if any, send over
    // Check for any cooking events that have just terminated
    // Unfreeze the player (set busy state to false), make the cookware ingredient free
    // Change the model of the cookware (if applicable)
    // Change status of ingredient and update the inventory of player
    // Check the instructions if player scored points for doing the correct instruction
    // If the instruction was sequential, give more points to the player
    std::vector<CookEvent*> & cookEvents = this->gameState.cookEvents;
    for (auto it = cookEvents.begin(); it != cookEvents.end(); )
    {
        CookEvent* cookEvent = *it;
        auto timeNow = std::chrono::high_resolution_clock::now();
        if (timeNow > cookEvent->terminationTime)
        {
            std::cout << "sufficient time has passed, cooking is complete" << std::endl;

            Ingredient* currIngredient = cookEvent->ingredient;
            Player* currPlayer = cookEvent->player;

            // Unfreeze the player
            cookEvent->player->setFreeze(false);

            /// TODO: make cookware ingredient free, set status dependent on food type
            cookEvent->cookware->setBusy(false);
            cookEvent->ingredient->setStatus(cookEvent->after);

            /// TODO: have to send over also (verify correctness of this)
            Game::ServerMessage* serverMsg = MessageBuilder::toInventoryServerMessage(currIngredient->getID(), true, currIngredient->getName()
                , currIngredient->getStatus(), currIngredient->getQualityIndex());
            this->specificMessages[currPlayer->getClientID()].push_back(serverMsg);

            // Send over the updated cookware object
            Game::ServerMessage* cookwareMsg = MessageBuilder::toServerMessage(cookEvent->cookware);
            this->messages.push_back(cookwareMsg);

            int i = 0;
            for( auto instruction : this->gameState.recipe->instructionList ) {
                if( (instruction->ingredient.compare(cookEvent->ingredient->getName()) == 0)
                && (instruction->cookware.compare(cookEvent->cookware->getName()) == 0 )
                && (instruction->after == cookEvent->after )) {

                    if( instruction->before == cookEvent->before )
                        cookEvent->player->addToScore(instruction->points);
                    else 
                        cookEvent->player->addToScore(instruction->points/2);
                    // Mark instruction as true
                    cookEvent->player->instructionSet.at(i) = std::make_pair(instruction, true);
                    
                    // sequential instruction bonus
                    if( i > 0 && cookEvent->player->instructionSet.at(i-1).second )
                        cookEvent->player->addToScore(instruction->points);
                    
                    serverMsg = MessageBuilder::toScore(cookEvent->player->getScore());
                    std::cout << "sending over a score" << std::endl;
                    this->specificMessages[currPlayer->getClientID()].push_back(serverMsg);
                }
                i++;
            }

            delete cookEvent;
            it = cookEvents.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Only spawn ingredient in dungeon phase
    if (this->gameState.getRound() == Game::RoundInfo::DUNGEON)
    {
        // Time to spawn ingredient
        auto currTime = std::chrono::high_resolution_clock::now();
        if (currTime > this->gameState.dungeonMap->ingredientSpawnTime)
        {
            int randNum = rand() % 100;
            Ingredient* ingredientCopy = NULL;

            // Spawn powerup
            if (randNum < Config::getInt("Vodka_Chance"))
            {
                ingredientCopy = RecipeBuilder::createIngredient(VODKA);
                ingredientCopy->setStatus(IngredientStatus::Delicious);
            }
            // Spawn ingredient
            else
            {
                // Get random ingredient
                std::vector<Ingredient*> & ingredientList = this->gameState.recipe->ingredientList;
                int randomIdx = rand() % ingredientList.size();
                Ingredient* currIngredient = ingredientList.at(randomIdx);

                // Make a copy
                ingredientCopy = RecipeBuilder::createIngredient(currIngredient->getName());

                // Random quality index
                ingredientCopy->randomizeQualityIndex();
            }

            // set spawn location
            int lowerX = this->gameState.dungeonMap->lowerX;
            int upperX = this->gameState.dungeonMap->upperX;
            int lowerZ = this->gameState.dungeonMap->lowerZ;
            int upperZ = this->gameState.dungeonMap->upperZ;

            bool isColliding = true;
            while (isColliding)
            {   
                // Choose a position
                int x = (rand() % (upperX - lowerX + 1)) + lowerX;
                int z = (rand() % (upperZ - lowerZ + 1)) + lowerZ;
                ingredientCopy->setPosition(glm::vec3(x, 0, z));
                
                // See if colliding
                isColliding = false;
                for (GameObject* obj : this->gameState.getAllObjects()) 
                {
                    if (ingredientCopy->isColliding(obj)) 
                    {
                        isColliding = true;
                        break;
                    }
                }
            }

            // Add to gameState, add to pending messages
            gameState.addIngredient(ingredientCopy);
            Game::ServerMessage* ingredientMsg = MessageBuilder::toServerMessage(ingredientCopy);
            this->messages.push_back(ingredientMsg);

            // Reset the time
            auto instantTime = std::chrono::high_resolution_clock::now();
            auto deltaTime = this->gameState.dungeonMap->timeDelta;
            this->gameState.dungeonMap->ingredientSpawnTime = instantTime + deltaTime;
        }
    }

    // Check power up times, remove if over
    for (auto it = this->gameState.clientPowerUpTimes.begin(); it != this->gameState.clientPowerUpTimes.end();)
    {
        auto endTime = it->second;
        auto nowTime = std::chrono::high_resolution_clock::now();

        // Power up is over, remove from map
        if (nowTime > endTime)
        {
            std::cout << "time is over, removing power up" << std::endl;
            Player* player = this->gameState.getPlayerObject(it->first);
            player->applyScale(glm::vec3(1));
            this->gameState.clientPowerUpTimes.erase(it++);

            Game::ServerMessage* serverMsg = MessageBuilder::toServerMessage(player);
            this->messages.push_back(serverMsg);
        }
        else
        {
            ++it;
        }
    }
}

void ServerGame::sendPendingMessages()
{
    // Send global messages
    for (auto message : this->messages) {
        this->server.sendToAll(*message);
        delete message;
    }
    this->messages.clear();

    // Send player specific messages
    for (auto pair : this->specificMessages)
    {
        auto clientId = pair.first;
        auto messages = pair.second;

        for (auto msg : messages)
        {
            this->server.send(clientId, *msg);
            delete msg;
        }

        messages.clear();
    }
    this->specificMessages.clear();
}

void ServerGame::onClientConnect(unsigned int clientId) 
{
    // Send lobby state to the client
    Game::ServerMessage* gameStatus = MessageBuilder::toRoundUpdate(this->gameState.getRound());
    this->server.send(clientId, *gameStatus);
    std::cout << gameStatus->DebugString() << std::endl;

    // Add client to ready map
    this->gameState.readyStatus[clientId] = false;

    // Add player with respective client ID
    Player* player = this->gameState.addPlayer(clientId);
    std::string configPath = "Player_" + std::to_string(clientId);
    player->setModel(Config::get(configPath));
   
    // while player is colliding, generate new location
    bool isColliding = true;
    while (isColliding)
    {
        // Choose a position
        int x = rand() % 6;
        int z = rand() % 6;
        player->setPosition(glm::vec3(x, 0, z));
                
        // See if colliding
        isColliding = false;
        for (GameObject* obj : this->gameState.getAllObjects()) 
        {
            if (obj == player) continue;
            if (player->isColliding(obj)) 
            {
                isColliding = true;
                break;
            }
        }
    }

    // Send objects
    for (GameObject* obj : this->gameState.getAllObjects()) 
    {
        Game::ServerMessage* message = MessageBuilder::toServerMessage(obj);
        this->server.sendToAll(*message);
        delete message;
    }

    // Send over client info
    Game::ServerMessage* clientInfoMsg = MessageBuilder::toClientInfo(clientId, player->getID());
    this->server.send(clientId, *clientInfoMsg);
    delete clientInfoMsg;
}

void ServerGame::onClientDisconnect(unsigned int clientId)
{
    std::cout << "servergame -> on client disconnect method is called" << std::endl;

    // Remove client from ready map
    this->gameState.readyStatus.erase(clientId);

    // Render player invisible
    Player* currPlayer = this->gameState.getPlayerObject(clientId);
    currPlayer->renderInvisible();

    //Send updated player to everyone else
    Game::ServerMessage* playerMessage = MessageBuilder::toServerMessage(currPlayer);
    this->server.sendToAll(*playerMessage);
    delete playerMessage;
    
    // Remove player from game state
    this->gameState.removePlayer(clientId);
}

void ServerGame::onRoundChange()
{
    // Send updated state to the client
    Game::ServerMessage* gameStatus = MessageBuilder::toRoundUpdate(this->gameState.getRound());
    this->server.sendToAll(*gameStatus);
    delete gameStatus;

    /// TODO: Handle unloading all objects. Should not unload the player object.

    // Init all objects into the world (everything preloaded)
    
    switch (this->gameState.getRound())
    {
        case Game::RoundInfo::LOBBY:
        {
            std::cout << "Initializing Lobby\n";
            break;
        }
        case Game::RoundInfo::DUNGEON_WAITING:
        { 
            std::cout << "Initializing dungeon waiting" << std::endl;

            // Initialize dungeon waiting time
            GameProcessor::initDungeonWaiting(&this->gameState);

            // Create the other phases (all visible)
            GameProcessor::initDungeonPhase(&this->gameState, this);
            GameProcessor::initKitchenPhase(&this->gameState);

            // Make kitchen invisible
            this->gameState.kitchenMap->setRender(false);

            /// Position players on spawn points
            GameProcessor::initPlayersLocations(gameState.dungeonMap, &gameState);
            break;
        }
        case Game::RoundInfo::DUNGEON:
        {
            std::cout << "initializing dungeon" << std::endl;
            gameState.setRoundTime(Config::getInt("Dungeon_Round_Time"));
            break;
        }
        case Game::RoundInfo::KITCHEN_WAITING:
        {
            std::cout << "initializing kitchen waiting" << std::endl;

            // Initialize kitchen waiting time
            GameProcessor::initKitchenWaiting(&this->gameState);
        
            // Make kitchen visible, dungeon invisible (and ingredients)
            this->gameState.kitchenMap->setRender(true);
            this->gameState.dungeonMap->setRender(false);
            for(auto ingredientPair : gameState.ingredientObjects )
                ingredientPair.second->setRender(false);

            // Position players on spawn points
            GameProcessor::initPlayersLocations(gameState.kitchenMap, &gameState);

            // Send instructions from recipe to clients
            int i = 0;
            for(auto inst : this->gameState.recipe->instructionList ) {
                Game::ServerMessage* msg = NULL;

                if(i == 0)
                    msg = MessageBuilder::toInstructionInfo(inst, i,  this->gameState.recipe->name);
                else
                    msg = MessageBuilder::toInstructionInfo(inst, i);
                this->messages.push_back(msg);
                i++;
            }
            // EndProcessor::initGameState(&this->gameState, this);
            break;
        }
        case Game::RoundInfo::KITCHEN:
        {
            std::cout << "initializing kitchen" << std::endl;
            gameState.setRoundTime(Config::getInt("Kitchen_Round_Time"));
            // "set render to false for everything not associate"
            break;
        }
        case Game::RoundInfo::END:
        {
            std::cout << "initializing end" << std::endl;
            GameProcessor::initEndPhase(&gameState, this);
            break;
        }
        default: 
        {
            std::cout << "Unhandled round change" << std::endl;
            break;
        }
    }

    // On round change, send over all objects
    for (GameObject* obj : this->gameState.getAllObjects()) {
        Game::ServerMessage* message = MessageBuilder::toServerMessage(obj);
        this->server.sendToAll(*message);
        delete message;
    }
}