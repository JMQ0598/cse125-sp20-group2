#include <objects/Player.h>

Player::Player(int ID) : GameObject(ID)
{
    //this->runSpeed = 0;
    //this->turnSpeed = 0;
    this->xSpeed = 0;
    this->zSpeed = 0;
    this->score = 0;
    this->objType = PLAYER;
    this->setModel(Config::get("Player_0"));
}

void Player::setScore(int newScore)
{
    this->score = newScore;
}

void Player::setClientID(int clientID)
{
    this->clientID = clientID;
}

void Player::setTeamName(std::string teamName)
{
    this->teamName = teamName;
}

void Player::addToScore(int addition)
{
    this->score += addition;
}

int Player::getScore()
{
    return this->score;
}

void Player::setXSpeed( float speed )
{
    this->xSpeed = speed;
}

float Player::getXSpeed()
{
    return this->xSpeed;
}

void Player::setZSpeed ( float speed )
{
    this->zSpeed = speed;
}

float Player::getZSpeed ()
{
    return this->zSpeed;
}

int Player::getClientID() { return this->clientID; };

std::string Player::getTeamName() { return this->teamName; }

void Player::setFreeze(bool f) {
    this->freeze = f;
}

bool Player::getFreeze() {
    return this->freeze;
}

Team Player::getTeam()
{
    return this->team;
}

void Player::setTeam(Team team)
{
    this->team = team;
}