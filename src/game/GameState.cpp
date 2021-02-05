#include <game/GameState.h>

#define PLAYER_RADIUS 0.05

GameState::GameState() {
    this->round = Game::RoundInfo::LOBBY;
}

GameState::~GameState() {
    for (auto objects : this->getAllObjects())
    {
        delete objects;
    }
}

Player* GameState::addPlayer(unsigned int clientId) {
    std::cout << "Called add user in GameState w/ clientID: " << clientId << std::endl;

    // First, create player w/ id
    int objId = this->objCounter++;
    Player* newPlayerObject = new Player(objId);
    newPlayerObject->setClientID(clientId);

    // Next, add player to map
    this->playerObjects[clientId] = newPlayerObject;
    return newPlayerObject;
}

void GameState::addTerrain(Map *map) {
    for(auto it = map->terrainList.begin(); it!= map->terrainList.end(); it++) {
        this->worldObjects[(*it)->getID()] = *it;
    }
}

void GameState::addIngredient(Ingredient* ing)
{
    this->ingredientObjects[ing->getID()] = ing;
} 

void GameState::addRecipe(Recipe *recipe) 
{
    this->recipe = recipe;
}

Recipe* GameState::getRecipe()
{
    return this->recipe;
}

const std::unordered_map<unsigned int, Player*>& GameState::getPlayerObjects()
{
    return this->playerObjects;
}

const std::unordered_map<unsigned int, Ingredient*>& GameState::getIngredientObjects() {
    return this->ingredientObjects;
}

Player* GameState::getPlayerObject(unsigned int clientId)
{
    return this->playerObjects[clientId];
}

Ingredient* GameState::getIngredientObject(unsigned int ingredientId)
{
    return this->ingredientObjects[ingredientId];
}

void GameState::removePlayer(unsigned int clientId)
{
    // Free player object, remove from player map
    Player* playerObject = this->playerObjects[clientId];
    delete playerObject;
    this->playerObjects.erase(clientId);
}

void GameState::removeIngredient(unsigned int ingredientId)
{
    Ingredient* ingredient = this->ingredientObjects[ingredientId];
    delete ingredient;
    this->ingredientObjects.erase(ingredientId);
}

/// TODO: REPAIR GAME OVER LOGIC
bool GameState::gameOver()
{
    switch (this->round)
    {
        // Game over if all players are ready
        case Game::RoundInfo_RoundState_LOBBY:
        {
            return this->allClientsReady();
        }
         // Game over if some time is over
        case Game::RoundInfo_RoundState_DUNGEON_WAITING:
        {
            break;
        }
        // Game over if some time is over
        case Game::RoundInfo_RoundState_DUNGEON:
        {
            break;
        }
        // Game over if 15 seconds is over
        case Game::RoundInfo_RoundState_KITCHEN_WAITING:
        {
            break;
        }
        // Game over if some time is over
        case Game::RoundInfo_RoundState_KITCHEN:
        {
            break;
        }
        // Never in game over
        case Game::RoundInfo_RoundState_END:
        {
            return false;
            break;
        }
        
        default:
            break;
    }

    auto currTime = std::chrono::high_resolution_clock::now();
    return currTime > this->roundEnd;
}

bool GameState::allClientsReady()
{
    // If no clients connected, return false
    if (this->readyStatus.size() == 0)
    {
        return false;
    }

    for (auto clientPair : this->readyStatus)
    {
        bool clientIsReady = clientPair.second;
        if (!clientIsReady)
        {
            return false;
        }
    }
    return true;
}

int GameState::getRoundTime()
{
    auto currTime = std::chrono::high_resolution_clock::now();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds> (this->roundEnd - currTime);

    return seconds.count() > 0 ? seconds.count() : 0;
}

bool GameState::timeHasUpdated()
{
    auto roundTime = this->getRoundTime();
    bool result = roundTime != this->oldTime;
    this->oldTime = roundTime;
    return result;
}

void GameState::removeAllObjects()
{
    this->ingredientObjects.clear();
    this->playerObjects.clear();
}

std::vector<GameObject*> GameState::getAllObjects()
{
    std::vector<GameObject*> gameObjectList;

    for (const auto & ingredientPair : this->ingredientObjects)
    {
        gameObjectList.push_back(ingredientPair.second);
    }

    for (const auto & playerPair : this->playerObjects)
    {
        gameObjectList.push_back(playerPair.second);
    }

    for (const auto & worldPair : this->worldObjects)
    {
        gameObjectList.push_back(worldPair.second);
    }

    for (const auto & cookwarePair : this->cookwareObjects)
    {
        gameObjectList.push_back(cookwarePair.second);
    }

    for (const auto & platePair : this->plateObjects)
    {
        gameObjectList.push_back(platePair.second);
    }

    return gameObjectList;
}

Game::RoundInfo::RoundState GameState::getRound()
{
    return this->round;
}


void GameState::setRound(Game::RoundInfo::RoundState round)
{
    this->round = round;
}

/// TODO: RESET BACK SO THAT LOBBY GOES TO DUNGEON WAITING INSTEAD OF DUNGEON
void GameState::advanceRound()
{
    switch (this->round)
    {
        case Game::RoundInfo::LOBBY :
        {
            this->round = Game::RoundInfo::DUNGEON_WAITING;
            break;
        }
        case Game::RoundInfo::DUNGEON_WAITING :
        {
            this->round = Game::RoundInfo::DUNGEON;
            break;
        }
        case Game::RoundInfo::DUNGEON :
        {
            this->round = Game::RoundInfo::KITCHEN_WAITING;
            break;
        }
        case Game::RoundInfo::KITCHEN_WAITING :
        {
            this->round = Game::RoundInfo::KITCHEN;
            break;
        }
        case Game::RoundInfo::KITCHEN :
        {
            this->round = Game::RoundInfo::END;
            break;
        }
        case Game::RoundInfo::END :
        {
            this->round = Game::RoundInfo::LOBBY;
            break;
        }
        default:
            break;
    }
}

void GameState::setRoundTime(unsigned int seconds)
{
    auto xMinutes = std::chrono::seconds(seconds);
    this->roundEnd = std::chrono::high_resolution_clock::now() + xMinutes;
    this->oldTime = 0;
}

Player* GameState::getWinningPlayer()
{
    int maxScore = -1;
    Player* maxPlayer = NULL;
    for (auto playerPair : this->playerObjects)
    {
        Player* currPlayer = playerPair.second;
        int currScore = currPlayer->getScore();

        if (currScore > maxScore)
        {
            maxScore = currScore;
            maxPlayer = currPlayer;
        }
    }
    return maxPlayer;
}