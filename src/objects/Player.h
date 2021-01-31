#pragma once

#include <objects/GameObject.h>
#include <objects/Ingredient.h>
#include <unordered_map>
#include <string>
#include <deque>

enum Team
{
    BLUE = 0,
    GREEN = 1,
    PURPLE = 2,
    RED = 3
};

class Player : public GameObject 
{
protected:
    int clientID;

    int score  = 0;

    bool freeze;

    std::string teamName;

    float xSpeed = 0, zSpeed = 0;

    Team team;

public:
    Player(int ID);

    void setFreeze(bool f);

    bool getFreeze();
    
    void setClientID( int clientID );

    void setTeamName( std::string teamName );

    int getClientID();

    void setXSpeed( float speed );

    //void setTurnSpeed( float newTurnSpeed );

    float getXSpeed();

    void setZSpeed ( float speed );

    float getZSpeed ();

    //float getTurnSpeed();

    std::string getTeamName();

    void setScore(int newScore);

    void addToScore( int addition );

    int getScore();

    Team getTeam();

    void setTeam(Team team);
};