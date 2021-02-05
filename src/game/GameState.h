#pragma once

#include <schema/Game.pb.h> 
#include <objects/GameObject.h>
#include <unordered_map>
#include <objects/Player.h>
#include <objects/Map.h>
#include <util/Recipe.h>
#include <utility>
#include <util/Config.h>
#include <game/CookEvent.h>
#include <chrono>
#include <objects/DungeonMap.h>
#include <objects/KitchenMap.h>
#include <objects/LobbyMap.h>

/**
 * This class holds the game state.
 * USE: ServerGame
 * */
class GameState {
private:
    const unsigned int VELOCITY = 1;

    // This is used to increment the counter
    unsigned int objCounter = 0;

    // This is a mapping of client id to Player objects
    /// TODO: Map from team id to allow instructor to quickly access their team's player
    std::unordered_map<unsigned int, Player*> playerObjects;

    // An enum representing the current round
    // Lobby - Represents phase where players are waiting
    // DungeonWaiting - This can be used to show a tutorial and memes
    //                  This should load any requirements for Dungeon
    // Dungeon - Represents phase of collecting ingredients
    // KitchenWaiting - This can be used to show a tutorial and memes
    //                  This should load any requirements for Kitchen
    // Kitchen - Represents the phase to combine ingredients
    Game::RoundInfo::RoundState round;

    // The time when the current round ends
    std::chrono::high_resolution_clock::time_point roundEnd;

    // Keeps track of previous time since last tick
    // Used to tell when a second has passed in GameState#timeHasUpdated
    int64_t oldTime;

public:
    GameState();
    ~GameState();

    ///TODO: Delete this if the game starts lagging hard
    LobbyMap* lobbyMap;
    DungeonMap* dungeonMap;
    KitchenMap* kitchenMap;

    std::vector<CookEvent*> cookEvents;

    // Mapping used to determine who is ready
    std::unordered_map<unsigned int, bool> readyStatus;

    // This is a mapping of gameObjId to cookware objects
    std::unordered_map<unsigned int, Cookware*> cookwareObjects;

    // This is a mapping of gameObjId to world objects
    std::unordered_map<unsigned int, GameObject*> worldObjects;

    // This is a mapping of gameObjId to ingredient objects
    std::unordered_map<unsigned int, Ingredient*> ingredientObjects;

    // This is a mapping of gameObjId to plate objects
    std::unordered_map<unsigned int, Plate*> plateObjects;

    // This is a priority queue of timer to clientId, to keep track of power ups
    std::unordered_map<unsigned int, std::chrono::high_resolution_clock::time_point> clientPowerUpTimes;

    Recipe* recipe; 

    /**
     * Adds a new user object using the clientId
     * */
    Player* addPlayer(unsigned int);

    /**
     * Gets a player object from the client id
     * */
    Player* getPlayerObject(unsigned int);

    /**
     * Return the reference ingredient object
     * */
    Ingredient* getIngredientObject(unsigned int);

    /**
     * Adds objects from maps
     * */
    void addTerrain(Map* map);

    void addIngredient(Ingredient* ing);

     /**
     * Add ingredient objects in map
     * */
    void addRecipe(Recipe *recipe);

    Recipe* getRecipe();

    /**
     * Returns all player objects
     * */
    const std::unordered_map<unsigned int, Player*>& getPlayerObjects();

    /**
     * Returns all ingredient objects
     * */
    const std::unordered_map<unsigned int, Ingredient*>& getIngredientObjects();

    /**
     * Removes a player object using the client id
     * */
    void removePlayer(unsigned int);

    /**
     * Remove a ingredient object using the object id
     * */
    void removeIngredient(unsigned int);

    /**
     * Used to determine if the current round is over
     * Logic is depended on the current round
     * */
    bool gameOver();

    /**
     * Used to determine if all clients are ready
     * */
    bool allClientsReady();

    /**
     * Used to retrieve the amount of time before the round ends.
     * This returns the time unit in seconds.
     * */
    int getRoundTime();

    /**
     * Used to determine if the timer value has updated, and should
     * be sent over to the clients
     * */
    bool timeHasUpdated();

    /**
     * Used to remove all objects
     * */
    void removeAllObjects();
    
    /**
     *  Used to get all objects
     * */
    std::vector<GameObject*> getAllObjects();

    /**
     * Used to retrieve the current round
     * */
    Game::RoundInfo::RoundState getRound();

    /**
     * Used to set the game state's round
     * */
    void setRound(Game::RoundInfo::RoundState);

    /**
     * Used to advance to the next round (for the gamestate)
     * */
    void advanceRound();

    /**
     * Used to set the round time (in seconds)
     * @param seconds the number of seconds from the current time that this round should end
     * */
    void setRoundTime(unsigned int seconds);

    /*
    * Used to get the winning player at the end of a round
    * */
   Player* getWinningPlayer();
};